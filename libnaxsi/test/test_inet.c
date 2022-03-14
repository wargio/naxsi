// SPDX-FileCopyrightText: 2022 wargio <deroad@libero.it>
// SPDX-License-Identifier: LGPL-3.0-only
#include "minunit_mem.h"
#include "naxsi_inet.h"
#include "naxsi_str.h"

#define test_naxsi_invalid_inet(ip) \
	do { \
		naxsi_str_init(&mem, &address, ip, strlen(ip), false); \
		inet = naxsi_inet_new(&mem, &address); \
		mu_assert_null(inet, "naxsi_inet_new(&mem, \"" ip "\") shall return NULL"); \
	} while (0)

#define test_naxsi_valid_inet4(ip, hex) \
	do { \
		naxsi_str_init(&mem, &address, ip, strlen(ip), false); \
		inet = naxsi_inet_new(&mem, &address); \
		mu_assert_notnull(inet, "naxsi_inet_new(&mem, \"" ip "\") shall NOT return NULL"); \
		mu_assert_eq_fmt(hex, inet->in.addr4, "valid ipv4 " ip, "0x%08x"); \
		naxsi_inet_free(&mem, inet); \
	} while (0)

#define test_naxsi_valid_inet6(ip, hex_hi, hex_lo) \
	do { \
		naxsi_str_init(&mem, &address, ip, strlen(ip), false); \
		inet = naxsi_inet_new(&mem, &address); \
		mu_assert_notnull(inet, "naxsi_inet_new(&mem, \"" ip "\") shall NOT return NULL"); \
		mu_assert_eq_fmt(hex_hi, inet->in.addr6[0], "valid ipv6 [hi] " #ip, "0x%016lx"); \
		mu_assert_eq_fmt(hex_lo, inet->in.addr6[1], "valid ipv6 [lo] " #ip, "0x%016lx"); \
		naxsi_inet_free(&mem, inet); \
	} while (0)

#define test_naxsi_invalid_subnet(cidr) \
	do { \
		naxsi_str_init(&mem, &address, cidr, strlen(cidr), false); \
		subnet = naxsi_subnet_new(&mem, &address); \
		mu_assert_null(subnet, "naxsi_subnet_new(&mem, \"" cidr "\") shall return NULL"); \
	} while (0)

#define test_naxsi_valid_subnet4(cidr, hexsub, hexmask) \
	do { \
		naxsi_str_init(&mem, &address, cidr, strlen(cidr), false); \
		subnet = naxsi_subnet_new(&mem, &address); \
		mu_assert_notnull(subnet, "naxsi_subnet_new(&mem, \"" cidr "\") shall NOT return NULL"); \
		mu_assert_eq_fmt(hexsub, subnet->subnet.addr4, "cidr subnet" #cidr, "0x%08x"); \
		mu_assert_eq_fmt(hexmask, subnet->submask.addr4, "cidr mask" #cidr, "0x%08x"); \
		naxsi_subnet_free(&mem, subnet); \
	} while (0)

#define test_naxsi_valid_subnet6(cidr, sub_hi, sub_lo, mask_hi, mask_lo) \
	do { \
		naxsi_str_init(&mem, &address, cidr, strlen(cidr), false); \
		subnet = naxsi_subnet_new(&mem, &address); \
		mu_assert_notnull(subnet, "naxsi_subnet_new(&mem, \"" cidr "\") shall NOT return NULL"); \
		mu_assert_eq_fmt(sub_hi, subnet->subnet.addr6[0], "cidr subnet [hi] " #cidr, "0x%016lx"); \
		mu_assert_eq_fmt(sub_lo, subnet->subnet.addr6[1], "cidr subnet [lo] " #cidr, "0x%016lx"); \
		mu_assert_eq_fmt(mask_hi, subnet->submask.addr6[0], "cidr mask [hi] " #cidr, "0x%016lx"); \
		mu_assert_eq_fmt(mask_lo, subnet->submask.addr6[1], "cidr mask [lo] " #cidr, "0x%016lx"); \
		naxsi_subnet_free(&mem, subnet); \
	} while (0)

#define test_naxsi_match_as(cidr, ip, expected) \
	do { \
		naxsi_str_init(&mem, &cidr_s, cidr, strlen(cidr), false); \
		naxsi_str_init(&mem, &addr_s, ip, strlen(ip), false); \
		subnet = naxsi_subnet_new(&mem, &cidr_s); \
		inet = naxsi_inet_new(&mem, &addr_s); \
		bool _res = naxsi_subnet_match(subnet, inet) == expected; \
		mu_assert("naxsi_subnet_match(\"" cidr "\", \"" ip "\") shall be " #expected, _res); \
		naxsi_inet_free(&mem, inet); \
		naxsi_subnet_free(&mem, subnet); \
	} while (0)

bool test_naxsi_methods(void) {
	naxsi_str_t string = naxsi_string("aaaaa");
	naxsi_inet_t *inet = NULL;
	naxsi_subnet_t *subnet = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* test subnet methods */
	subnet = naxsi_subnet_new(&mem, NULL);
	mu_assert_null(subnet, "naxsi_subnet_new(NULL, NULL) shall return NULL");

	subnet = naxsi_subnet_new(NULL, &string);
	mu_assert_null(subnet, "naxsi_subnet_new(NULL, \"\") shall return NULL");

	subnet = naxsi_subnet_new(&mem, &string);
	mu_assert_null(subnet, "naxsi_subnet_new(&mem, \"aaaaa\") shall return NULL");

	/* test inet methods */
	inet = naxsi_inet_new(NULL, &string);
	mu_assert_null(inet, "naxsi_inet_new(NULL, \"\", 5, false) shall return NULL");

	inet = naxsi_inet_new(&mem, NULL);
	mu_assert_null(inet, "naxsi_inet_new(&mem, \"\") shall return NULL");

	inet = naxsi_inet_new(&mem, &string);
	mu_assert_null(inet, "naxsi_inet_new(&mem, \"aaaaa\") shall return NULL");

	/* test subnet match */
	mu_assert_false(naxsi_subnet_match(NULL, NULL), "naxsi_subnet_match(NULL, NULL) shall return false and not crash");
	mu_assert_false(naxsi_subnet_match((naxsi_subnet_t *)42, NULL), "naxsi_subnet_match(42, NULL) shall return false and not crash");
	mu_assert_false(naxsi_subnet_match(NULL, (naxsi_inet_t *)42), "naxsi_subnet_match(NULL, 42) shall return false and not crash");
	mu_end;
}

bool test_naxsi_inet4(void) {
	naxsi_str_t address;
	naxsi_inet_t *inet = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* valid ipv4 */
	test_naxsi_valid_inet4("1.1.1.1", 0x01010101);
	test_naxsi_valid_inet4("163.89.3.2", 0xa3590302);
	test_naxsi_valid_inet4("128.173.4.29", 0x80ad041d);
	test_naxsi_valid_inet4("239.255.255.255", 0xefffffff);
	test_naxsi_valid_inet4("134.15.99.29", 0x860f631d);
	test_naxsi_valid_inet4("28.51.100.98", 0x1c336462);
	test_naxsi_valid_inet4("109.42.4.69", 0x6d2a0445);
	test_naxsi_valid_inet4("187.252.38.144", 0xbbfc2690);
	test_naxsi_valid_inet4("69.218.17.94", 0x45da115e);
	test_naxsi_valid_inet4("88.102.174.189", 0x5866aebd);
	test_naxsi_valid_inet4("189.242.49.90", 0xbdf2315a);
	test_naxsi_valid_inet4("153.219.215.206", 0x99dbd7ce);
	test_naxsi_valid_inet4("79.70.250.237", 0x4f46faed);

	/* invalid ipv4 */
	test_naxsi_invalid_inet("1.1.1.423");
	test_naxsi_invalid_inet("aaaaaa");
	test_naxsi_invalid_inet("okdsa.23.3.1");
	test_naxsi_invalid_inet("");
	test_naxsi_invalid_inet("0.0.0.0");
	test_naxsi_invalid_inet("240.0.0.0");
	test_naxsi_invalid_inet("250.0.7.1");
	test_naxsi_invalid_inet("255.255.255.255");

	mu_end;
}

bool test_naxsi_inet6(void) {
	naxsi_str_t address;
	naxsi_inet_t *inet = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* valid ipv6 */
	test_naxsi_valid_inet6("2001:1::2", 0x2001000100000000ul, 0x0000000000000002ul);
	test_naxsi_valid_inet6("2001:0001:0000:0000:0000:0000:0000:0002", 0x2001000100000000ul, 0x0000000000000002ul);
	test_naxsi_valid_inet6("::1", 0ul, 0x1ul);
	test_naxsi_valid_inet6("0000:0000:0000:0000:0000:0000:0000:0001", 0ul, 0x1ul);
	test_naxsi_valid_inet6("35cf:bbfa:7e5c:c145:81cc:11d4:2286:8b0b", 0x35cfbbfa7e5cc145ul, 0x81cc11d422868b0bul);
	test_naxsi_valid_inet6("8c68:1bf8:4d96:581f:6992:1cf8:eb95:f2fe", 0x8c681bf84d96581ful, 0x69921cf8eb95f2feul);
	test_naxsi_valid_inet6("4460:a59d:6f57:c0bd:df24:07e5:3c85:4aae", 0x4460a59d6f57c0bdul, 0xdf2407e53c854aaeul);
	test_naxsi_valid_inet6("4182:a877:9321:3323:d811:d0f6:0517:2972", 0x4182a87793213323ul, 0xd811d0f605172972ul);
	test_naxsi_valid_inet6("cb55:a760:8b24:fee7:7393:b70b:8d75:5538", 0xcb55a7608b24fee7ul, 0x7393b70b8d755538ul);
	test_naxsi_valid_inet6("0de3:12b1:e915:101d:5ac3:2374:23d5:e36c", 0x0de312b1e915101dul, 0x5ac3237423d5e36cul);
	test_naxsi_valid_inet6("6a32:3010:cd8c:2282:cb43:3a6d:aaa5:7801", 0x6a323010cd8c2282ul, 0xcb433a6daaa57801ul);
	test_naxsi_valid_inet6("2695:e707:9c2a:7754:502f:9e74:8808:d54e", 0x2695e7079c2a7754ul, 0x502f9e748808d54eul);
	test_naxsi_valid_inet6("b42a:7d2f:ce4a:7b80:97f0:6447:ec39:c580", 0xb42a7d2fce4a7b80ul, 0x97f06447ec39c580ul);
	test_naxsi_valid_inet6("b42a:7d2f:ce4a:0:97f0:6447:ec39:c580", 0xb42a7d2fce4a0000ul, 0x97f06447ec39c580ul);
	test_naxsi_valid_inet6("b42a:7d2f:ce4a::97f0:6447:ec39:c580", 0xb42a7d2fce4a0000ul, 0x97f06447ec39c580ul);

	/* invalid ipv6 */
	test_naxsi_invalid_inet("::");
	test_naxsi_invalid_inet("ff00::");
	test_naxsi_invalid_inet("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	test_naxsi_invalid_inet("ffff:0000:0000:0000:ffff:ffff:ffff:c580");
	test_naxsi_invalid_inet("b42a:7d2f:ce4a:::6447:ec39:c580");
	test_naxsi_invalid_inet("b42a:7d2f:ce4a::aaaa:6447::c580");

	mu_end;
}

bool test_naxsi_subnet4(void) {
	naxsi_str_t address;
	naxsi_subnet_t *subnet = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* valid ipv4 CIDRs */
	test_naxsi_valid_subnet4("0.0.0.0/1", 0x00000000, 0x80000000);
	test_naxsi_valid_subnet4("155.206.121.20/2", 0x80000000, 0xc0000000);
	test_naxsi_valid_subnet4("10.0.0.0/8", 0xa000000, 0xff000000);
	test_naxsi_valid_subnet4("169.254.0.0/16", 0xa9fe0000, 0xffff0000);
	test_naxsi_valid_subnet4("192.0.0.171/32", 0xc00000ab, 0xffffffff);
	test_naxsi_valid_subnet4("88.102.174.189/9", 0x58000000, 0xff800000);
	test_naxsi_valid_subnet4("79.70.250.237/22", 0x4f46f800, 0xfffffc00);
	test_naxsi_valid_subnet4("255.255.255.255/32", 0xffffffff, 0xffffffff);

	/* invalid ipv4 CIDRs */
	test_naxsi_invalid_subnet("0.0.0.0/0");
	test_naxsi_invalid_subnet("");
	test_naxsi_invalid_subnet("1.1.1/.423");
	test_naxsi_invalid_subnet("aaaaaa/22");
	test_naxsi_invalid_subnet("okdsa.23.3.1/");
	test_naxsi_invalid_subnet("8.2uu3.3.1/2");
	test_naxsi_invalid_subnet("64.91.171.38");
	test_naxsi_invalid_subnet("166.139.151.29/63");
	test_naxsi_invalid_subnet("106.58.215.16/0");

	mu_end;
}

bool test_naxsi_subnet6(void) {
	naxsi_str_t address;
	naxsi_subnet_t *subnet = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* valid ipv6 CIDRs */
	test_naxsi_valid_subnet6("2001:1::1/128", 0x2001000100000000lu, 0x1lu, 0xfffffffffffffffflu, 0xfffffffffffffffflu);
	test_naxsi_valid_subnet6("2001:4860:4860::8888/32", 0x2001486000000000lu, 0lu, 0xffffffff00000000lu, 0lu);
	test_naxsi_valid_subnet6("2001:4:112::/48", 0x2001000401120000lu, 0lu, 0xffffffffffff0000lu, 0lu);
	test_naxsi_valid_subnet6("2001:1::2/63", 0x2001000100000000lu, 0lu, 0xfffffffffffffffelu, 0lu);
	test_naxsi_valid_subnet6("0de3:12b1:e915:101d:5ac3:2374:23d5:e36c/120", 0x0de312b1e915101dul, 0x5ac3237423d5e300ul, 0xfffffffffffffffflu, 0xffffffffffffff00lu);
	test_naxsi_valid_subnet6("b42a:7d2f:ce4a::97f0:6447:ec39:c580/103", 0xb42a7d2fce4a0000ul, 0x97f06447ec000000ul, 0xfffffffffffffffflu, 0xfffffffffe000000lu);
	test_naxsi_valid_subnet6("::/1", 0lu, 0lu, 0x8000000000000000lu, 0lu);

	/* invalid ipv6 CIDRs */
	test_naxsi_invalid_subnet("::/0");
	test_naxsi_invalid_subnet("2001:1::2/182");
	test_naxsi_invalid_subnet("2001:1::2/");
	test_naxsi_invalid_subnet("2001:iooo::2/22");

	mu_end;
}

bool test_naxsi_match(void) {
	naxsi_str_t cidr_s, addr_s;
	naxsi_inet_t *inet = NULL;
	naxsi_subnet_t *subnet = NULL;
	naxsi_mem_t mem = mu_naxsi_mem();

	/* ipv4 */
	test_naxsi_match_as("127.0.0.0/8", "127.0.1.1", true);
	test_naxsi_match_as("192.0.0.170/32", "192.0.0.170", true);
	test_naxsi_match_as("192.0.0.170/32", "192.0.0.171", false);

	/* ipv6 */
	test_naxsi_match_as("::1/128", "::1", true);
	test_naxsi_match_as("::/128", "::1", false);
	test_naxsi_match_as("2001:4:112::/48", "2001:4:112::e3", true);
	test_naxsi_match_as("0de3:12b1:e915:101d:5ac3:2374:23d5:e300/120", "0de3:12b1:e915:101d:5ac3:2374:23d5:e36c", true);

	/* mixed */
	test_naxsi_match_as("192.0.0.170/32", "2001:4:112::e3", false);
	test_naxsi_match_as("::/10", "64.91.171.38", false);

	mu_end;
}

int all_tests() {
	mu_run_test(test_naxsi_methods);
	mu_run_test(test_naxsi_inet4);
	mu_run_test(test_naxsi_inet6);
	mu_run_test(test_naxsi_subnet4);
	mu_run_test(test_naxsi_subnet6);
	mu_run_test(test_naxsi_match);
	return tests_passed != tests_run;
}

mu_main(all_tests)
