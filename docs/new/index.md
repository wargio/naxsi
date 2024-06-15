# **Naxsi Web Application Firewall Documentation**

## What is a Web Application Firewall (WAF)?

﻿A Web Application Firewall (WAF) is a security system that sits in front of a web application to inspect, filter, and block malicious traffic. It acts as an intermediary between the internet and your website or web application, examining HTTP requests and responses for potential threats.

One of the key features of WAFs is virtual patching, which allows them to protect against known vulnerabilities without requiring actual patches to be applied to the underlying code. Here's how it works on Naxsi:

1. **Rule-based detection**: The WAF has a list of known patterns, i.e. malicious request, which are used to block requests.
2. **Request analysis**: When an incoming HTTP request is received, the WAF analyzes its contents against the list of patterns.
3. **Threat identification**: If the request matches a known pattern, the WAF first checks if there is a whitelist rule and if not, then it assigns a score to the request; if this score exceeds the score limits then the request is blocked.

Naxsi is also capable to employ a whitelisting strategy, where all incoming traffic is initially blocked by default. In this case, only the requests that match specific rules are then allowed to pass through and reach the web application behind the WAF. This approach provides an added layer of security by assuming all unknown traffic poses a potential threat until proven otherwise.

## Virtual patching

Sometimes it happens that the web application behind the WAF might have a security vulnerability that cannot be patched immediately by the system owner; Naxsi can apply "virtual patches" to block the vulnerability without requiring changes to the underlying web application code.

By using virtual patching, the system owner can:

* Protect against zero-day attacks and unknown vulnerabilities
* Prevent exploitation of unpatched or outdated software versions
* Reduce the attack surface by blocking suspicious requests before they reach your website

These virtual patches are expressed in form of Naxsi rules and can be applied to RAW requests or specific fields within the request.

# Getting Started

* **Installing Naxsi**: [Learn how to compile and install Naxsi from source code.](build-naxsi.md)
* **Basic Configuration**: [A basic configuration for Naxsi](basic-configuration.md).

# Configuration Options

* **Directives**: Explains all the directives that are available when the Naxsi module is enabled. 
* **Rules**: Understand the different types of rules you can create in Naxsi, including signature-based and anomaly-based detection.
* **Whitelists**: Learn how to use whitelisting features in Naxsi to allow specific traffic through your firewall.
* **Matchzones**: Describes how matchzones can be used to limit rules or whitelists.

# Integrating with Other Tools

* **Integration with a Dashboard**: Discover how to integrate Naxsi with other tools and dashboards for enhanced monitoring and management capabilities.
