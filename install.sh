#!/bin/sh

fatal() {
	echo "$@"
	exit 1
}

dir=$(dirname $0)

sed -i -e "s|HOME|$HOME|g" "$dir/lib/config.hpp" || fatal "Failed to set home directory"

make clean || fatal "Failed to clean junkdb"
make || fatal "Failed to build junkdb"

if [[ -d ~/.junkdb ]]; then
	rm -rf ~/.junkdb || fatal "Failed to remove ~/.junkdb"
fi

mkdir ~/.junkdb || fatal "Failed to create ~/.junkdb"
mkdir ~/.junkdb/data || fatal "Failed to create ~/.junkdb/data"

cp "$dir/server/bin" ~/.junkdb/junkdb || fatal "Failed to copy junkdb server"
cp "$dir/client/bin" ~/.junkdb/junkdb-cli || fatal "Failed to copy junkdb client"

if ! [[ "$(cat ~/.bashrc)" == *junkdb* ]]; then
	echo -e 'export PATH="$PATH:$HOME/.junkdb"' >>~/.bashrc || fatal "Failed to add junkdb to PATH"
fi

sed -i -e "s|$HOME|HOME|g" "$dir/lib/config.hpp" || fatal "Failed to set home directory"
