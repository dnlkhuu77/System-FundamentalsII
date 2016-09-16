#include "utfconverter.h"

int main(int argc, char** argv){
	unsigned char buf[2];
	int fd = 0;
	int rv = 0;
	Glyph* glyph= NULL;
	/* After calling parse_args(), filename and conversion should be set. */
	parse_args(argc, argv);
	fd = open(filename, O_RDONLY); 

	glyph = malloc(sizeof(Glyph));
	
	/* Handle BOM bytes for UTF16 specially. 
         * Read our values into the first and second elements. */
	if((rv = read(fd, &buf[0], 1)) == 1 && (rv = read(fd, &buf[1], 1)) == 1){

		if(buf[0] == 0xfe && buf[1] == 0xff){
			/*file is big endian*/
			source = BIG; 
		} else if(buf[0] == 0xff && buf[1] == 0xfe){
			/*file is little endian*/
			source = LITTLE;
		} else {
			/*file has no BOM*/
			free(glyph); 
			fprintf(stderr, "File has no BOM.\n");
			quit_converter(NO_FD); 
		}

		memset(glyph, 0, sizeof(Glyph));
	}

	fill_glyph(glyph, buf, source, &fd);
		if(conversion != LITTLE)
			swap_endianness(glyph);
	write_glyph(glyph);


	/* Now deal with the rest oconversionf the bytes.*/
	while((rv = read(fd, &buf[0], 1)) == 1 &&
			(rv = read(fd, &buf[1], 1)) == 1){

		glyph = fill_glyph(glyph, buf, source, &fd);
		if(conversion!= LITTLE)
			glyph = swap_endianness(glyph);
		write_glyph(glyph);

	}

	free(glyph);
	quit_converter(NO_FD);
	return 0;
}

Glyph* swap_endianness(Glyph* glyph){
	/* Use XOR to be more efficient with how we swap values. */
	unsigned char helper;
	helper = glyph->bytes[0];
	glyph->bytes[0] = glyph->bytes[1];
	glyph->bytes[1] = helper;
	if(glyph->surrogate){  /* If a surrogate pair, swap the next two bytes. */
		helper = glyph->bytes[2];
		glyph->bytes[2] = glyph->bytes[3];
		glyph->bytes[3] = helper;
	}
	glyph->end = conversion;
	return glyph;
}

Glyph* fill_glyph(Glyph* glyph, unsigned char data[2], endianness end, int* fd){
		unsigned int bits;

	if(end == LITTLE){
		glyph->bytes[0] = data[0];
		glyph->bytes[1] = data[1];
	}
	else if (end == BIG){
		glyph->bytes[0] = data[1]; 
		glyph->bytes[1] = data[0];
	}
	bits = 0; 
	bits |= ((data[FIRST] << 8 ) + data[SECOND]);
	/* Check high surrogate pair using its special value range.*/

	if(bits > 0xD800 && bits < 0xDBFF){ 
		/*fprintf(stderr, "STUFFFFFFF\n" );*/
		if(read(*fd, &data[FIRST], 1) == 1 && read(*fd, &data[SECOND], 1) == 1){
			bits = 0;
			bits |= ((data[FIRST] << 8) + data[SECOND]);


			if(bits > 0xDC00 && bits < 0xDFFF){
				glyph->surrogate = true; 
			} else {
				printf("Invalid\n");
				print_help();
			}
		}

	}else
		glyph->surrogate = false;


	if(!glyph->surrogate){
		glyph->bytes[THIRD] = '\0';
		glyph->bytes[FOURTH] = '\0';
	} else {
		glyph->bytes[THIRD] = data[FIRST]; 
		glyph->bytes[FOURTH] = data[SECOND];
	}

	glyph->end = end;

	return glyph;
}

void write_glyph(Glyph* glyph){
	if(glyph->surrogate){ 
		write(STDOUT_FILENO, glyph->bytes, SURROGATE_SIZE); 
	} else {
		write(STDOUT_FILENO, glyph->bytes, NON_SURROGATE_SIZE);
	}
}

void parse_args(int argc, char** argv){
	int c, option_index = 0;
	char* endian_convert = NULL; 

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"h", no_argument, 0, 'h'},
		{"u", required_argument, 0, 'u'},
		{0, 0, 0, 0}
	};
	
	/* If getopt() returns with a valid (its working correctly) 
	 * return code, then process the args! */

	if((c = getopt_long(argc, argv, "hu:", long_options, &option_index)) != -1){
		switch(c){ 
			case 'u':
				endian_convert = optarg;
				break;
			case 'h':
				print_help();
				break;
			default:
				fprintf(stderr, "Unrecognized argument.\n");
				print_help();
				quit_converter(NO_FD);
				break;
		}
	}

	if(optind < argc){
		strcpy(filename, argv[optind]);
	} else {
		fprintf(stderr, "Filename not given.\n");
		print_help();
	}

	if(endian_convert == NULL){
		fprintf(stderr, "Converson mode not given.\n");
		print_help();
	}

	if(strcmp(endian_convert, "16LE") == 0){
		conversion = LITTLE;
	} else if(strcmp(endian_convert, "16BE") == 0){
		conversion = BIG;
	} else {
		quit_converter(NO_FD);
	}
}

void print_help(void) {
	int j = 0;
	for(j = 0; j < 4; j++){
		printf("%s", USAGE[j]); 
	}
	quit_converter(NO_FD);
}

void quit_converter(int fd){
	close(STDERR_FILENO);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	if(fd != NO_FD)
		close(fd);
	exit(0);
	/* Ensure that the file is included regardless of where we start compiling from. */
}