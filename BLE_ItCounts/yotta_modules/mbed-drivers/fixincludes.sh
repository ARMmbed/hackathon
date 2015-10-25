#!/bin/sh

OLDDIR=$1
NEWDIR=$2

EREFLAG='-r'
unamestr=`uname`

if [[ $unamestr == 'Darwin' ]]; then
    EREFLAG='-E'
elif [[ $unamestr == 'FreeBSD' ]]; then
    EREFLAG='-E'
fi

if [ -z "$OLDDIR" ]; then
    echo "$0 <olddir> <newdir>"
    exit 1
fi

if [ -z "$NEWDIR" ]; then
    echo "$0 <olddir> <newdir>"
    exit 1
fi

FILES=`find $OLDDIR -type f`
for FILE in $FILES; do
    # echo "$FILE"
    GUARD=`echo "__${FILE}__" | tr '[a-z]' '[A-Z]' | tr '.\\/-' '____'`
    DEST=`echo "$FILE" | sed $EREFLAG -e "s|^(./)?$OLDDIR|$NEWDIR|"`
    DESTDIR=`dirname $DEST`
    DESTGUARD=`echo "__${DEST}__" | tr '[a-z]' '[A-Z]' | tr '.\\/-' '____'`
    mkdir -p $DESTDIR
    git mv $FILE $DEST
    sed -i .old -e "s/${GUARD}/${DESTGUARD}/" $DEST
    rm $DEST.old
    git add $DEST
done

git commit -m "Moved include files to new location"

for FILE in $FILES; do
    DEST=`echo "$FILE" | sed $EREFLAG -e "s|^(./)?$OLDDIR|$NEWDIR|"`
    GUARD=`echo "__${FILE}__" | tr '[a-z]' '[A-Z]' | tr '.\\/-' '____'`
    cat << EOF > $FILE
/*
 * Copyright (c) 2015, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ${GUARD}
#define ${GUARD}
#warning ${FILE} is deprecated.  Please use ${DEST} instead.
#include "../${DEST}"
#endif // ${GUARD}

EOF
    git add $FILE
done

git commit -m "Added include forwarders"
