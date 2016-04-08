#!/bin/bash
if [[ -z $1 ]]; then
    echo "Usage: $0 <test>"
    exit 1
fi
pushd . > /dev/null
cd `dirname $0`
curpath=`pwd -P`
cd ..
./runsbox "$curpath/$1/test" 1000 65536 /dev/null "$curpath/$1/given_output" 2>/dev/null
echo $? > "$curpath/$1/given_exitcode"
cd "$curpath"

if ! cmp "$1/given_exitcode" "$1/exitcode" > /dev/null 2>&1; then
    echo "  FAIL  $1"
    exit 1
fi

if ! cmp "$1/given_output" "$1/output" > /dev/null 2>&1; then
    echo "  FAIL  $1"
    exit 1
fi
echo "  PASS  $1"
popd > /dev/null
