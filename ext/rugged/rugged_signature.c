/*
 * The MIT License
 *
 * Copyright (c) 2012 GitHub, Inc
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "rugged.h"

VALUE rugged_signature_new(const git_signature *sig, const char *encoding_name)
{
	VALUE rb_sig, rb_time;

#ifdef HAVE_RUBY_ENCODING_H
	rb_encoding *encoding = NULL;

	if (encoding_name != NULL)
		encoding = rb_enc_find(encoding_name);
#endif

	rb_sig = rb_hash_new();

	rb_time = rb_time_new(sig->when.time, 0);
	rb_funcall(rb_time, rb_intern("utc"), 0);

	rb_hash_aset(rb_sig, CSTR2SYM("name"), rugged_str_new2(sig->name, encoding));
	rb_hash_aset(rb_sig, CSTR2SYM("email"), rugged_str_new2(sig->email, encoding));
	rb_hash_aset(rb_sig, CSTR2SYM("time"), rb_time);

	return rb_sig;
}

git_signature *rugged_signature_get(VALUE rb_sig)
{
	int error;
	VALUE rb_time, rb_unix_t, rb_offset, rb_name, rb_email;
	git_signature *sig;

	Check_Type(rb_sig, T_HASH);

	rb_name = rb_hash_aref(rb_sig, CSTR2SYM("name"));
	rb_email = rb_hash_aref(rb_sig, CSTR2SYM("email"));
	rb_time = rb_hash_aref(rb_sig, CSTR2SYM("time"));

	Check_Type(rb_name, T_STRING);
	Check_Type(rb_email, T_STRING);
	if (!rb_obj_is_kind_of(rb_time, rb_cTime))
		rb_raise(rb_eTypeError, "expected Time object");

	rb_unix_t = rb_funcall(rb_time, rb_intern("tv_sec"), 0);
	rb_offset = rb_funcall(rb_time, rb_intern("utc_offset"), 0);

	error = git_signature_new(&sig,
		StringValueCStr(rb_name),
		StringValueCStr(rb_email),
		NUM2LONG(rb_unix_t),
		FIX2INT(rb_offset) / 60);

	rugged_exception_check(error);

	return sig;
}

