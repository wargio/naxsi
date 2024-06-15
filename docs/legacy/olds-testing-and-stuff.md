# Unit Tests

Naxsi comes with [unit tests]( https://github.com/nbs-system/naxsi/tree/master/t ).
This is very useful for people who wants to modify naxsi, build packages, test it on exotic setups, …
We try to keep our [code coverage]( http://codecov.io/github/nbs-system/naxsi?branch=master ) as high as possible.

This is how you can run them:

1. Untar [nginx code source]( http://nginx.org/en/download.html ) in `/tmp/nginx/`.
2. Setup naxsi for unit testing (from [naxsi_src]( https://github.com/nbs-system/naxsi/tree/master/naxsi_src )).
 * `make re` will compile naxsi to run from `/tmp/`
 * `make deploy ` will deploy a minimal naxsi in `/tmp/naxsi_ut/` (running on high port)
 * `make test` will run the unit tests
3. It will give you a pass/fail result.
4. Look at the resulting `/tmp/naxsicov.html/` to get a code coverage report.

If you're too lazy to run them, no worries, [travis-ci]( https://travis-ci.org/nbs-system/naxsi ) is running them for every pull-request that you'll submit!

## Coverity

[Coverity]( https://scan.coverity.com/projects/1883 ) is cool enough to allow open-source projects to use their cloud solutions for free. We do submit builds for every release. If you want to have access to detail results (ie. doing security assessment on naxsi), get in touch with us!