# Classical benchmark : Web Vuln Scanner vs WAF

Here we will present the results of (IBM) [AppScan]( https://www-03.ibm.com/software/products/en/appscan/ ) vs naxsi.

Why AppScan ? Because, it's one of the less worse commercial scanner available, because they offer a target/demo (read : highly vulnerable) website, and, unlike [Acunetix]( https://www.acunetix.com/ ), you can run full tests on the target website.

# Overall Results 

As you will see bellow, AppScan is not able to find any vulnerabilities that it can exploit. The only vulnerabilities detected are probed with extremely simple patterns (read : nearly full ascii) that are not (and will not) be blocked by naxsi (to avoid false positives).

## Raw results

When running AppScan against [demo.testfire.net]( http://demo.testfire.net ) with naxsi (set-up as forward proxy for this purpose), the following vulnerabilities will appear:

[[Images/appscanfull.png|appscanfull]]

I won't make an exhaustive diff list between naxsi and raw results since, as you can expect, original scan results have a LOT of vulnerabilities, as this website is designed to show all the vulnerabilities AppScan can detect/exploit.

Let's go through all the "remaining" bugs :

## Blind SQL Injections

The scan result shows at least 2 Blind SQL Injection remaining. Actually, the attack pattern used by AppScan is the following :

[[Images/appscandetails1.png|details 1]]
[[Images/appscandetails2.png|details 2]]


The first test pattern `listAccounts=0%2B0%2B0%2B1001160141` is decoded to `listAccounts=0+0+0+1001160141`, and the seconde one, `before=1234+and+7659%3D7659` to `before=1234 and 7659=7659`. Appscan is able to detect the vulnerability, but it will not be able to exploit it.

## Database Error Pattern

During the scan, AppScan was able to detect several "Database Error Pattern" (read : MSSQL/MySQL/... error messages in webpage)

[[Images/appscandetails3.png|details 3]]

The test pattern used by AppScan is `100116014WFXSSProbe`, it's a generic one to *trace back* injected data into a webpage. Here, the scanner is able to trigger the bug by entering a non-numeric value in a numeric field, making the page yield a SQL error message. Obviously, you cannot do anything here except blocking the scanner's pattern, which would be dumb and inefficient. Obviously, the injected characters won't allow any kind of exploitation of the vulnerability, only a probe.

## Other vulnerabilities

The other vulnerabilities reported by AppScan :

* Weak credentials : Because it's possible to login with admin/admin. No comment Out of scope for a WAF.
* Session Reuse : When logging / logging off and logging in again, AppScan notices that the Cookie (session) doesn't change. (Out of scope for a WAF)
* Weak Cookies : Sensitive information is stored in the user's cookie, enabling cookie theft if someone can access the user's workstation. (Out of scope for a WAF)
* No account lock : An attacker can brute-force passwords. (Out of scope for a WAF, but nginx with req_limit might help you solving that !)
* CSRF : This is more touchy, because it can be blocked by NAXSI, by adding a rule like `BasicRule "rx:http://demo.testfire.net/*" "mz:$HEADERS:Referer";`, but we didn't add it, because we wanted this scan to be really "out of the box" with no customization.