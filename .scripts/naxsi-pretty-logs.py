#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2024 deroad <wargio@libero.it>
# SPDX-License-Identifier: LGPL-3.0-only

import sys
import json

EPILOG='''
Invalid usage.

This tool prettify naxsi json error logs
Example:
	$ python naxsi-pretty-logs.py error0.log error1.log ...
'''

def terminate_json(jsonstr):
	inner = 0
	string = False
	escape = False
	comma = False
	idx = 0
	split = 0
	for ch in jsonstr:
		print("--------------")
		print(ch)
		print("escape", escape)
		print("string", string)
		print("comma", comma)
		print("inner", inner)
		idx += 1
		if ch == '{' and not string:
			inner += 1
		elif ch == '}' and not string:
			inner -= 1
			if inner < 1:
				return jsonstr[:idx]
		elif ch == '\\' and not escape:
			escape = True
			continue
		elif ch == ',' and not escape and not string:
			comma = True
			continue
		elif ch == '"' and not escape:
			if not string:
				split = idx - 1
			else:
				print(jsonstr[split:idx])
			string = not string
		escape = False
		comma = False

	print("escape", escape)
	if escape:
		jsonstr += '\\'

	print("string", string)
	if string:
		jsonstr += '"'

	print("inner", inner)
	return jsonstr + ("}" * inner)


def prettify_error_log(file):
	with open(file, "rb") as fp:
		while True:
			line = fp.readline().decode('utf-8')
			if not line:
				break
			elif '{"ip"' not in line:
				continue

			jsonstr = line[line.index('{"ip"'):]
			if '"}' not in jsonstr:
				jsonstr = terminate_json(jsonstr)
				print(jsonstr)
				return
			continue
			try:
				print('OK:', json.loads(jsonstr))
			except Exception:
				print('XX:', jsonstr)

def main(argv):
	if len(argv) < 2:
		print(EPILOG)
		return

	for file in argv:
		prettify_error_log(file)

if __name__ == '__main__':
	main(sys.argv)