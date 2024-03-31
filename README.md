# Junk DB

A trivial database to store script states

When using  the default [*configuration*](./lib/config.hpp), a junk database has a ***8KB* fixed size**, storing a **single numeric status** and *1009* **hashes of generic data**.

The latter cannot be read back, but only added, removed or checked for existence. **Older entries are overwritten** when the database limit is exceeded. Databases are **automatically created** on first access.

## Installation

#### Linux

```
$ git clone https://github.com/9elt/junkdb
$ cd junkdb
$ sh install.sh
```

This will locally install `junkdb`, `junkdb-cli` and set up your `data` directory.

## Protocol

```
<database> <ACTION> <payload>
```

#### Actions

`GET`    Get the database *status*

`SET`    Set the database *status*, the *payload* must be an integer

`HAS`    Check if the database contains the *payload*

`ADD`    Add the *payload* to the database

`REM`    Remove the *payload* from the database

`DEL`    Delete the database

#### Example

```
my-databse SET 32
```

## Client

#### Usage

```
junkdb-cli <database> [options] <payload> [options]
junkdb-cli <request> [options] --raw [options]

--strip,   -s    strip response status
--raw            execute raw request
--get,     -G    get the database status
--set,     -S    set the database status
--has,     -H    check if the database contains the payload
--add,     -A    add the payload to the database
--remove,  -R    remove the payload from the database
--delete,  -D    delete the datbase
--help,    -h    show this help message
--version, -V    show junkdb-cli version
```

#### Example

```
$ junkdb-cli 'my-database' --set 32
```
```
$ junkdb-cli 'my-database' --get
OK 32
```
```
$ junkdb-cli 'my-database' --has 'Hello World!'
OK false
```
```
$ junkdb-cli 'my-database' --add 'Hello World!'
OK
```
```
$ junkdb-cli 'my-database' --has 'Hello World!'
OK true
```
```
$ junkdb-cli 'my-database' --remove 'Hello World!'
OK
```
```
$ junkdb-cli 'my-database' --delete
OK
```

