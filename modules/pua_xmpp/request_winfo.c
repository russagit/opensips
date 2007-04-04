/*
 * $Id: request_winfo.c 1666 2007-03-02 13:40:09Z anca_vamanu $
 *
 * pua_xmpp module - presence SIP - XMPP Gateway
 *
 * Copyright (C) 2007 Voice Sistem S.R.L.
 *
 * This file is part of openser, a free SIP server.
 *
 * openser is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version
 *
 * openser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * History:
 * --------
 *  2007-03-29  initial version (anca)
 */

#include <stdio.h>
#include <stdlib.h>
#include <libxml/parser.h>

#include "pua_xmpp.h"
#include "pidf.h"
#include "request_winfo.h"

#define PRINTBUF_SIZE 256

int request_winfo(struct sip_msg* msg, char* uri, char* expires)
{
	subs_info_t subs;
	struct sip_uri puri;
	int printbuf_len;
	char buffer[PRINTBUF_SIZE];
	str uri_str;

	memset(&puri, 0, sizeof(struct sip_uri));
	if(uri)
	{
		printbuf_len = PRINTBUF_SIZE-1;
		if(xl_printf(msg, (xl_elem_t*)uri, buffer, &printbuf_len)<0)
		{
			LOG(L_ERR, "PUA_XMPP: send_winfo: ERROR cannot print the format\n");
			return -1;
		}
		if(parse_uri(buffer, printbuf_len, &puri)!=0)
		{
			LOG(L_ERR, "PUA_XMPP: send_winfo: bad owner SIP address!\n");
			goto error;
		} else
		{
			DBG("PUA_XMPP: send_winfo: using user id [%.*s]\n", printbuf_len,
					buffer);
		}
	} 
	if(puri.user.len<=0 || puri.user.s==NULL
			|| puri.host.len<=0 || puri.host.s==NULL)
	{
		LOG(L_ERR, "PUA_XMPP: send_winfo: bad owner URI!\n");
		goto error;
	}
	uri_str.s= buffer;
	uri_str.len=  printbuf_len;
	DBG("pua_xmpp: xmpp_send_winfo  uri= %.*s:\n", uri_str.len, uri_str.s);

	memset(&subs, 0, sizeof(subs_info_t));
	
	subs.pres_uri= &uri_str;

	subs.watcher_uri= &uri_str;

	subs.contact= &uri_str;
	
	if(strncmp(expires, "0", 1 )== 0)
	{
		subs.expires= 0;
	}
	else
	{
		subs.expires= -1;
	
	}
	/* -1 - for a subscription with no time limit */
	/*  0  -for unsubscribe */

	subs.source_flag |= XMPP_SUBSCRIBE;
	subs.event= PWINFO_EVENT;

	if(pua_send_subscribe(&subs)< 0)
	{
		LOG(L_ERR, "pua_xmpp: xmpp_send_winfo:ERROR while sending subscribe\n");
		goto error;
	}

	return 1;

error:
	return 0;
}

