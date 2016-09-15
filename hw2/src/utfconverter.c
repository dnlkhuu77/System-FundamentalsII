#include "utfconverter.h"

//all declarations in header file. DELETE //COMMENTS FOR SPARKY
//bin/utfconverter -u LE rsrc/utf16le.txt

int main(int argc, char** argv){
	/* After calling parse_args(), filename and conversion should be set. */
	parse_args(argc, argv);
	int fd = open(filename, O_RDONLY); 
	unsigned char buf[2]; //store 
	int rv = 0;

	Glyph* glyph = malloc(sizeof(Glyph));
	
	/* Handle BOM bytes for UTF16 specially. 
         * Read our values into the first and second elements. */
	if((rv = read(fd, &buf[0], 1)) == 1 && (rv = read(fd, &buf[1], 1)) == 1){

		if(buf[0] == 0xfe && buf[1] == 0xff){ //big
			/*file is little endian*/
			source = BIG; 
		} else if(buf[0] == 0xff && buf[1] == 0xfe){
			/*file is big endian*/
			source = LITTLE;
		} else {
			/*file has no BOM*/
			free(glyph); 
			fprintf(stderr, "File has no BOM.\n");
			quit_converter(NO_FD); 
		}

		//memset(glyph, 0, sizeof(Glyph));
	}

	glyph = fill_glyph(glyph, buf, source, &fd); //print the BOM
		if(conversion == BIG)
			glyph = swap_endianness(glyph);
	write_glyph(glyph);


	/* Now deal with the rest oconversionf the bytes.*/
	while((rv = read(fd, &buf[0], 1)) == 1 && //read two bytes (code pt), pass to fill_byte 
			(rv = read(fd, &buf[1], 1)) == 1){

		glyph = fill_glyph(glyph, buf, source, &fd);
		if(conversion == BIG)
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

	//for little endians (add conditional of source to see what edianness)
	if(end == LITTLE){
		glyph->bytes[0] = data[0]; //filling the glyph with what you read
		glyph->bytes[1] = data[1];
	}
	else if (end == BIG){
		glyph->bytes[0] = data[1]; //filling the glyph with what you read
		glyph->bytes[1] = data[0];
	}

	unsigned int bits = 0; 
	bits |= (data[FIRST] + (data[SECOND] << 8)); //for little endian (|= means OR)
	/* Check high surrogate pair using its special value range.*/

	if(bits > 0xD800 && bits < 0xDBFF){ 
		if(read(*fd, &data[SECOND], 1) == 1 && read(*fd, &data[FIRST], 1) == 1){
			bits |= (data[FIRST] + (data[SECOND] << 8));

			if(bits > 0xDC00 && bits < 0xDFFF){
				glyph->surrogate = true; 
			} else {
				lseek(*fd, -OFFSET, SEEK_CUR); 
				glyph->surrogate = false;
			}
		}

	}

	if(!glyph->surrogate){
		glyph->bytes[THIRD] = glyph->bytes[FOURTH] |= 0;
	} else {
		glyph->bytes[THIRD] = data[FIRST]; 
		glyph->bytes[FOURTH] = data[SECOND];
	}
	glyph->end = end;

	return glyph;
}

void write_glyph(Glyph* glyph){
	//if(glyph->surrogate){ //if the glyph is a surrogate
		write(STDOUT_FILENO, glyph->bytes, SURROGATE_SIZE); //std int file number
	//} else {
	//	write(STDIN_FILENO, glyph->bytes, NON_SURROGATE_SIZE);
	//}
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
	
	//option_index replace long_options
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

	if(strcmp(endian_convert, "LE") == 0){ 
		conversion = LITTLE;
	} else if(strcmp(endian_convert, "BE") == 0){
		conversion = BIG;
	} else {
		quit_converter(NO_FD);
	}
}

void print_help(void) {
	for(int i = 0; i < 4; i++){
		printf("%s", USAGE[i]); 
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