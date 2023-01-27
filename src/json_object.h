/*
 * Copyright (C) 2016-2017 Redis Labs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <jsonsl.h>
#include <math.h>
#include "rmutil/sds.h"
#include <stdlib.h>
#include "object.h"
#include "rmstrndup.h"
#include "redismodule.h"

#define JSONOBJECT_OK 0
#define JSONOBJECT_ERROR 1

#define JSONOBJECT_MAX_ERROR_STRING_LENGTH 256

/* A custom context for the JSON parser. */
typedef struct {
    jsonsl_error_t err;  // parser error
    size_t errpos;       // error position
    Node **nodes;        // stack of created nodes
    int nlen;            // size of node stack
} _JsonParserContext;

/* A context for JSON objects. */
typedef struct {
    int levels;                // the maximum number of levels up to JSONSL_MAX_LEVELS, 0 for that
    jsonsl_t parser;           // the parser
    _JsonParserContext *pctx;  // the parser's custom context
} JSONObjectCtx;

JSONObjectCtx *NewJSONObjectCtx(int levels);
void FreeJSONObjectCtx(JSONObjectCtx *ctx);

/**
 * Parses a JSON stored in `buf` of size `len` and creates an object.
 * The resulting object tree is stored in `node` and in case of error the optional `err` is set with
 * the relevant error message.
 *
 * Note: the JSONic 'null' is represented internally as NULL, so `node` can be NULL even when the
 *       return code is JSONOBJECT_OK.
 */
int CreateNodeFromJSON(JSONObjectCtx *ctx, const char *buf, size_t len, Node **node, char **err);

typedef struct {
    char *indentstr;   // indentation string
    char *newlinestr;  // linebreak string
    char *spacestr;    // spacing before/after element in size=1 containers, and after keys
    int noescape;      // Don't return escape in output
} JSONSerializeOpt;

/**
 * Produces a JSON serialization from an object.
 */
void SerializeNodeToJSON(const Node *node, const JSONSerializeOpt *opt, sds *json);
sds JSONSerialize_String(sds buf, const char *s, size_t len, int noescape);
#endif