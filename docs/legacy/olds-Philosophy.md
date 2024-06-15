## Naxsi overview

Technically, *naxsi* is a third party [nginx]( https://nginx.org ) module, available as a package for many UNIX-like platforms. This module, by default, reads a small subset of [simple rules]( https://github.com/nbs-system/naxsi/blob/master/naxsi_config/naxsi_core.rules ) containing known patterns involved in websites vulnerabilities and exploitation. For example, `&lt;`, `|` or `drop` are not supposed to be part of a URI.

Being very simple, those patterns may match legitimate queries, it is naxsi's administrator duty to add specific rules that will [[whitelist|whitelists]] those legitimate behaviours. The administrator can either add whitelists manually by analysing nginx's error log, or (recommended) start the project by an intensive auto-learning phase that will automatically generate whitelisting rules regarding website's behaviour.

In short, Naxsi behaves like a *DROP-by-default* firewall, the only job needed is to add required ACCEPT rules for the target website to work properly.

## Naxsi expected usage

As Naxsi relies on whitelists to avoid false positives (even more than with a "classic" WAF), it comes with a tool called [nxapi]( https://github.com/nbs-system/naxsi/tree/master/nxapi ) that helps the administrator to create the appropriate whitelists for an application.
If you are using common web applications 'out of the box', you might as well find pre-generated whitelists [here]( https://github.com/nbs-system/naxsi-rules )

## Naxsi Pro's and Con's
Naxsi relies on a positive model, which has multiple advantages, but comes with some extra constraints :

#### Pros
* **Fast** : Minimalist, Lightweight runtime processing
* **Resilient** : Signature-less design allows improved resilience against [[obfuscated / complex attacks|NaxsiVSObfuscated]]
* **Update independent** : Signature-less design allows sustainable security, even without updates

#### Con's
* **Configuration** : Positive approach requires a heavier whitelisting process than negative model
* **Not Idiot Proof** : As naxsi behaves like a network firewall, if you are setting up too laxist rules, your web application will not be protected correctly.
