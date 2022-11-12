/*
 * BSD 3-Clause License
 * 
 * Copyright (c) 2022, uidops
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <sys/mman.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_VERSION "Brainfuck Interpreter - v0.1"

#define FUCK_CASES switch (*(fuckcode+i)) { \
				case '>': \
					(*memory)++; \
					break; \
				case '<': \
					(*memory)--; \
					break; \
				case '+': \
					(**memory)++; \
					break; \
				case '-': \
					(**memory)--; \
					break; \
				case '.': \
					fputs(*memory, stdout); \
					break; \
				case ',': \
					**memory = getchar(); \
					break; \
				case '[': \
					i += fuckloop(fuckcode+i, memory); \
					break; \
				default: \
					continue; \
			}


void		 fuckhelp(const char *);
unsigned	 fucklinter(char *, size_t);
size_t		 fucklen(const char *);
size_t		 fuckloop(const char *, char **);
void		 fuckinter(char **, const char *, size_t);


int
main(int argc, const char **argv)
{
	const char *fname = NULL;
	char *fp = NULL, *_memory = NULL;
	char **memory = NULL;
	int fd = -1;
	off_t length = 0;

	if (argc < 2 || argc > 3) {
		fuckhelp(*argv);
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < argc; i++) {
		if (!strcmp(*(argv+i), "-h")) {
			fuckhelp(*argv);
			exit(EXIT_SUCCESS);

		} else if (!strcmp(*(argv+i), "-v")) {
			puts(PROGRAM_VERSION);
			exit(EXIT_SUCCESS);


		} else if (*(*(argv+i)) == '-') {
			fuckhelp(*argv);
			exit(EXIT_FAILURE);

		} else {
			if (fname != NULL) {
				fuckhelp(*argv);
				exit(EXIT_FAILURE);
			}

			fname = *(argv+i);
		}
	}

	fd = open(*(argv+1), O_RDONLY);
	if (fd == -1)
		err(EXIT_FAILURE, "open");

	length = lseek(fd, 0, SEEK_END);
	fp = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
	if (fp == MAP_FAILED)
		err(EXIT_FAILURE, "mmap");

	if (!fucklinter(fp, length))
		exit(EXIT_FAILURE);

	memory = calloc(sizeof(char *), 2);
	if (memory == NULL)
		err(EXIT_FAILURE, "malloc");

	*memory = calloc(sizeof(char), 5120);
	if (*memory == NULL)
		err(EXIT_FAILURE, "malloc");

	_memory = *memory;

	fuckinter(memory, fp, length);
	putchar(0x0a);

	if (munmap(fp, length) == -1)
		err(EXIT_FAILURE, "munmap");

	free(_memory);
	free(memory);

	return EXIT_SUCCESS;
}


void
fuckhelp(const char *program_name)
{
	puts(PROGRAM_VERSION);
	printf("usage: %s [-hv] [script]\n", program_name);
	puts("\n -h: HELP\n -v: VERSION");

	return;
}


size_t
fucklen(const char *fuckcode)
{
	size_t flag = 0, n = 0;

	while (*(fuckcode+n)) {
		if (*(fuckcode+n) == '[')
			flag++;
		else if (flag && *(fuckcode+n) == ']')
			flag--;
		else if (!flag && *(fuckcode+n) == ']')
			break;

		n++;
	}

	return n;
}


size_t
fuckloop(const char *fuckcode, char **memory)
{
	size_t length = fucklen(++fuckcode);

	while (**memory) {
		for (size_t i = 0; i < length; i++)
			FUCK_CASES
	}

	return length+1;
}


void
fuckinter(char **memory, const char *fuckcode, size_t length)
{
	for (size_t i = 0; i < length; i++)
			FUCK_CASES

	return;
}


unsigned
fucklinter(char *fuckcode, size_t length)
{
	unsigned f = 1;
	long long l = 0;

	for (size_t i = 0; i < length; i++) {
		switch (*(fuckcode+i)) {
			case '[' :
				l++;
				break;
			case ']' :
				l--;
				break;
			case '>' :
			case '<' :
			case '+' :
			case '-' :
			case '.' :
			case ',' :
			case ' ' :
			case '\t':
			case '\n':
			case '\v':
			case '\f':
			case '\r':
				break;
			default:
				warnx("Error at %zu: Undefined command '%c'", i+1, *(fuckcode+i));
				f = 0;
		}
	}

	if (l)
		warnx("Error: Excepted %c", (l < 0) ? '[' : ']');
	
	return f&!l;
}
