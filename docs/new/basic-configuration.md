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
	include /etc/nginx/naxsi_core.rules; # Include core rules

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

		# include additional rules
		include /etc/nginx/additional_naxsi.rules;

		# internal denied request.
		DeniedUrl "/RequestDenied";

		CheckRule "$SQL >= 8" BLOCK;
		CheckRule "$RFI >= 8" BLOCK;
		CheckRule "$TRAVERSAL >= 5" BLOCK;
		CheckRule "$UPLOAD >= 5" BLOCK;
		CheckRule "$XSS >= 8" BLOCK;
		CheckRule "$UWA >= 8" BLOCK;
		CheckRule "$EVADE >= 8" BLOCK;
		CheckRule "$LIBINJECTION_XSS >= 8" BLOCK;
		CheckRule "$LIBINJECTION_SQL >= 8" BLOCK;
	}
	location /RequestDenied {
		internal;
		return 403;
	}
}
```

This configuration enables NAXSI and sets up basic rules for blocking requests based on various threat levels. The `SecRulesEnabled` directive is mandatory to enable NAXSI in a location.

Some key directives used in this example include:

* `DeniedUrl`: specifies where blocked requests will be redirected (**this is an internal redirect for NGINX**).
* `LearningMode`: if enabled, `BLOCK` CheckRule will be considered as `LOG`, thus not blocking the requests.
* `CheckRule`: takes an action (`LOG`, `BLOCK`, `DROP`, `ALLOW`) based on a specific score associated with the request.
* `include`: This directive allows to include other configuration files within the current scope, this can be useful if the system owner wants to have the same configuration for multiple websites without copy-pasting the same lines.

Additionally, this configuration includes directives for enabling libinjection's XSS and SQLI detection features.

Note 1: **Be aware that Nginx will fail to load the configuration, if `ngx_http_naxsi_module.so` is not loaded**.
Note 2: It is possible to test the NGINX configuration by using `nginx -t` from the command line.