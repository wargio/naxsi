# **Naxsi Basic Configuration**

To get started with Naxsi, you can explore the following basic configuration.

This NGINX configuration for `/etc/nginx/nginx.conf` where we define a reverse proxy towards a webservice hosted on `internal-ip-address` on port `80`.

```
# load module
load_module /etc/nginx/modules/ngx_http_naxsi_module.so;

server {
	listen 80;
	server_name example.com;

	set $naxsi_json_log 1; # Enable JSON logs for Naxsi
	include /etc/nginx/naxsi/naxsi_core.rules; # Include core rules (see below)

	location / {
		proxy_pass http://internal-ip-address:80;
		proxy_set_header Host $host;
		proxy_set_header X-Real-IP $remote_addr;
		proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
		proxy_set_header X-Forwarded-Proto $scheme;

		SecRulesEnabled; #enable naxsi for this `location`
		# LearningMode;  #When enable, BLOCK CheckRule are considered as LOG.
		LibInjectionSql; #enable libinjection support for SQLI
		LibInjectionXss; #enable libinjection support for XSS

		# Internal denied request.
		DeniedUrl "/RequestDenied";

		# Include additional rules
		include /etc/nginx/naxsi/blocking/*.rules;

		# The following CheckRules are mandatory when using the rules found in the naxsi repository.
		# For more info, please check:
		# - https://github.com/wargio/naxsi/tree/main/naxsi_rules/blocking
		# - https://github.com/wargio/naxsi/blob/main/naxsi_rules/naxsi_core.rules

		CheckRule "$SQL >= 8" BLOCK; # SQL injection action (unrelated to libinjection)
		CheckRule "$XSS >= 8" BLOCK; # XSS action (unrelated to libinjection)
		CheckRule "$RFI >= 8" BLOCK; # Remote File Inclusion action
		CheckRule "$UWA >= 8" BLOCK; # Unwanted Access action
		CheckRule "$EVADE >= 8" BLOCK; # Evade action (some tools may try to avoid detection).
		CheckRule "$UPLOAD >= 5" BLOCK; # Malicious upload action
		CheckRule "$TRAVERSAL >= 5" BLOCK; # Traversal access action
		CheckRule "$LIBINJECTION_XSS >= 8" BLOCK; # libinjection XSS action
		CheckRule "$LIBINJECTION_SQL >= 8" BLOCK; # libinjection SQLi action
	}

	# The location where all the blocked request will be internally redirected.
	location /RequestDenied {
		internal;
		return 403;
	}
}
```

This configuration enables NAXSI and sets up basic rules for blocking requests based on various threat levels.

> 📣 Important
>
> The `SecRulesEnabled` directive is mandatory to enable NAXSI in a location.

Some key directives used in this example include:

* `DeniedUrl`: specifies where blocked requests will be redirected (**this is an internal redirect for NGINX**).
* `LearningMode`: if enabled, `BLOCK` CheckRule will be considered as `LOG`, thus not blocking the requests.
* `CheckRule`: takes an action (`LOG`, `BLOCK`, `DROP`, `ALLOW`) based on a specific score associated with the request.
* `include`: This directive allows to include other configuration files within the current scope, this can be useful if the system owner wants to have the same configuration for multiple websites without copy-pasting the same lines.

Additionally, this configuration includes directives for enabling libinjection's XSS and SQLI detection features.

> ⚠️ Warning
>
> **Be aware that Nginx will fail to load the configuration, if `ngx_http_naxsi_module.so` is not loaded**.

> 💡 Tip
>
> It is possible to test the NGINX configuration by using `nginx -t` from the command line.

# Go Back

[Table of Contents](index.md).
