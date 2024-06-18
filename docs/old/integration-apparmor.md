# Introduction
If you care about process access control on a [AppArmor]( http://apparmor.net ) enabled *GNU/Linux* platform, and you are using Naxsi, this article is for you.

Of course defining a AppArmor profile is not straight forward and must be customized. AppArmor developers know that and provided tools easing the creation of application profiles.

Of course, path to files may change depending on your local setup.

# Requirements

You basically need:
* A GNU/Linux platform with kernel support for AppArmor, [Ubuntu]( http://www.ubuntu.com/ ) for example
* `apt-get install apparmor-utils`
* A running Nginx/Naxsi
* A couple of minutes

# Generating the profile

As root, run the following command to generate a profile:

```
aa-genprof /usr/local/sbin/nginx
```

Then answer the few questions:

```
    Connecting to repository.....

    WARNING: Error fetching profiles from the repository:
    RPC::XML::Client::send_request: HTTP server error: Not Found

    Writing updated profile for /usr/local/sbin/nginx.
    Setting /usr/local/sbin/nginx to complain mode.

    Please start the application to be profiled in
    another window and exercise its functionality now.

    Once completed, select the "Scan" button below in
    order to scan the system logs for AppArmor events.

    For each AppArmor event, you will be given the
    opportunity to choose whether the access should be
    allowed or denied.

    Profiling: /usr/local/sbin/nginx

    [(S)can system log for SubDomain events] / (F)inish
```

Now stop, start, reload, restart Nginx in order to generate apparmor logs. Then press 'S'

```
    Reading log entries from /var/log/messages.
    Updating AppArmor profiles in /etc/apparmor.d.
    Complain-mode changes:

    Profile:  /usr/local/sbin/nginx
    Path:     /etc/nginx/mime.types
    Mode:     owner r
    Severity: unknown

     [1 - /etc/nginx/mime.types]

     [(A)llow] / (D)eny / (G)lob / Glob w/(E)xt / (N)ew / Abo(r)t / (F)inish / (O)pts
```

Now we are starting to profile the program and it is then really up to you to decide which file need to be accessed, with which access rights, by Nginx.

Once done with answering those questions, a apparmor profile for `/usr/local/sbin/nginx` will be created in `/etc/apparmor.d/usr.local.sbin.nginx`.

Here is mine:

```apparmor
    # Last Modified: Wed Jul 25 11:33:59 2012
    #include <tunables/global>

    /usr/local/sbin/nginx {
      #include <abstractions/apache2-common>
      #include <abstractions/base>
      #include <abstractions/nameservice>
  
      capability dac_override,
      capability dac_read_search,
      capability setgid,
      capability setuid,

      owner /etc/nginx/** r,
      owner /etc/ssl/openssl.cnf r,
      /var/lib/certificates/* r,
      owner /var/log/nginx/* a,
      /var/log/nginx/* w,
      owner /var/run/nginx.pid rw,
      /var/www/** r,
    }
```

# Updating the profile for some time
Unless you precisely know what the program is doing it is a good idea to let it live for some time and gather the logs that will improve your Apparmor profile.

So we activate the complain mode of AppArmor:

    aa-complain /etc/apparmor.d/usr.local.sbin.nginx

And after some time (depending on your usage of Nginx):

    aa-logprof /etc/apparmor.d/usr.local.sbin.nginx

This command will simply go through the logs and ask you about updating Nginx profile.

# Enforcing the profile and open up Naxsi to tougher guys

    aa-enforce /etc/apparmor.d/usr.local.sbin.nginx

Yep that's all!

BUT! Unauthorized access caught by AppArmor in enforce mode will NOT generate you extra logs! For that you need the “audit” mode (aa-audit). Sadly the audit mode also logs authorized access so it can rapidly become unreadable (you read dmesg output all the time don't you?)

Enjoy Naxsi and Apparmor!