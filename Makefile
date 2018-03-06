all:
	mkdir -p _build
	gcc -Wall -Werror -shared -o _build/dovecot_hash.so src/dovecot_hash.c -fPIC

clean:
	rm -rf _build
