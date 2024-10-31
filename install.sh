#!/bin/sh

fail() {
	echo "Fatal error: $@"
	exit 1
}

dir=$(dirname $0)

sed -i -e "s|HOME|$HOME|g" "$dir/src/config.h" || fail "failed to set home directory"

make || fail "failed to build junkdb"

sed -i -e "s|$HOME|HOME|g" "$dir/src/config.h" || fail "failed to reset config"

if [[ -d ~/.junkdb ]]; then
	rm -rf ~/.junkdb || fail "failed to remove existing ~/.junkdb"
fi

mkdir "$HOME/.junkdb" || fail "failed to create new ~/.junkdb"
mkdir "$HOME/.junkdb/data" || fail "failed to create ~/.junkdb/data"

cp "$dir/junkdb" "$HOME/.junkdb/junkdb" || fail "failed to copy junkdb server"
cp "$dir/junkdb-cli" "$HOME/.junkdb/junkdb-cli" || fail "failed to copy junkdb client"

if ! [[ "$(cat "$HOME/.bashrc")" == *junkdb* ]]; then
	echo -e 'export PATH="$PATH:$HOME/.junkdb"' >>"$HOME/.bashrc" || fail "failed to add junkdb to path"
fi

make clean || fail "failed to make clean"
