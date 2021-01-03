run:
	gcc -I/usr/local/Cellar/hidapi/0.10.1/include/hidapi \
		-L/usr/local/Cellar/hidapi/0.10.1/lib \
		-lhidapi \
		-framework ApplicationServices \
		-o driver driver.c
