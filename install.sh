#!/bin/sh

fail() {
	echo "$@"
	exit 1
}

dir=$(dirname $0)

sed -i -e "s|HOME|$HOME|g" "$dir/lib/config.hpp" || fail "Failed to set home directory"

make clean || fail "Failed to clean junkdb"
make || fail "Failed to build junkdb"

sed -i -e "s|$HOME|HOME|g" "$dir/lib/config.hpp" || fail "Failed to reset config"

if [[ -d ~/.junkdb ]]; then
	rm -rf ~/.junkdb || fail "Failed to remove ~/.junkdb"
fi

mkdir ~/.junkdb || fail "Failed to create ~/.junkdb"
mkdir ~/.junkdb/data || fail "Failed to create ~/.junkdb/data"

cp "$dir/junkdb/bin" ~/.junkdb/junkdb || fail "Failed to copy junkdb server"
cp "$dir/junkdb-cli/bin" ~/.junkdb/junkdb-cli || fail "Failed to copy junkdb client"

if ! [[ "$(cat ~/.bashrc)" == *junkdb* ]]; then
	echo -e 'export PATH="$PATH:$HOME/.junkdb"' >>~/.bashrc || fail "Failed to add junkdb to PATH"
fi
