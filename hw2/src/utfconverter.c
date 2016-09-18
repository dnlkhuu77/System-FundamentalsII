#include "utfconverter.h"

int fd, fd2 = 0;
int v_counter = 0;
float ascii_count, ascii_total = 0;
float surr_count, surr_total = 0;
int glyph_total = 0; /*We are counting the BOM*/

int main(int argc, char** argv){
	unsigned char buf[2];
	int rv = 0;
	Glyph* glyph= NULL;

	/* After calling parse_args(), filename and conversion should be set. */
	parse_args(argc, argv);
	fd = open(filename, O_RDONLY);
	if(filename2 != NULL)
		fd2 = open(filename2, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IROTH | S_IRGRP
			| S_IWUSR | S_IWGRP | S_IWOTH);

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
			quit_converter(fd);
			quit_converter(fd2); 
		}
		memset(glyph, 0, sizeof(Glyph));
	}

	fill_glyph(glyph, buf, source, &fd);
		if(conversion != LITTLE)
			swap_endianness(glyph);
	write_glyph(glyph,fd2);


	/* Now deal with the rest oconversionf the bytes.*/
	while((rv = read(fd, &buf[0], 1)) == 1 &&
			(rv = read(fd, &buf[1], 1)) == 1){

		if(buf[0] < 128 && buf[1] < 128)
			ascii_count++;
		ascii_total++;

		glyph = fill_glyph(glyph, buf, source, &fd);
		if(conversion!= LITTLE)
			glyph = swap_endianness(glyph);
		write_glyph(glyph,fd2);

	}

	free(glyph);

	if(v_counter == 1)
		verb1(filename);
	else if (v_counter > 1)
		verb2(filename);

	quit_converter(fd);
	quit_converter(fd2);

	return EXIT_SUCCESS;
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
				surr_count++;
			} else {
				printf("Invalid\n");
				print_help();
			}
		}

	}else
		glyph->surrogate = false;
	surr_total++;


	if(!glyph->surrogate){
		glyph->bytes[FOURTH] = '\0';
		glyph->bytes[THIRD] = '\0';
	} else {
		if(end == BIG){
			glyph->bytes[THIRD] = data[SECOND]; 
			glyph->bytes[FOURTH] = data[FIRST];
			glyph_total++;
			ascii_total++;
			surr_total++;
		}
		else{
			glyph->bytes[THIRD] = data[FIRST]; 
			glyph->bytes[FOURTH] = data[SECOND];
		}
	}

	glyph->end = end;
	glyph_total++;
	return glyph;
}

void write_glyph(Glyph* glyph, int fd_2){
	printf("fd_2: %d\n", fd_2);
	if(fd_2 != 0){
		if(glyph->surrogate){ 
			write(fd_2, glyph->bytes, SURROGATE_SIZE);
		} else {
			write(fd_2, glyph->bytes, NON_SURROGATE_SIZE);
		}
	}
	else if (fd_2 == 0){
		if(glyph->surrogate){ 
			write(STDOUT_FILENO, glyph->bytes, SURROGATE_SIZE);
		} else {
			write(STDOUT_FILENO, glyph->bytes, NON_SURROGATE_SIZE);
		}
	}
}

void parse_args(int argc, char** argv){
	int c, option_index = 0;
	char* endian_convert = NULL; 

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"h", no_argument, 0, 'h'},
		{"UTF=", required_argument, 0, 'u'},
		{"u", required_argument, 0, 'u'},
		{"v", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};
	
	/* If getopt() returns with a valid (its working correctly) 
	 * return code, then process the args! */

	while((c = getopt_long(argc, argv, "hvu:", long_options, &option_index)) != -1){
		switch(c){ 
			case 'u':
				endian_convert = optarg;
				break;
			case 'h':
				print_help();
				break;
			case 'v':
				v_counter++;
				break;
			default:
				fprintf(stderr, "Unrecognized argument.\n");
				print_help();
				quit_converter(fd);
				quit_converter(fd2);
				break;
		}
	}

	if(optind < argc){
			strcpy(filename, argv[optind]);
	} else {
		fprintf(stderr, "Filename not given.\n");
		print_help();
	}

	if(argv[optind + 1] != NULL){
		strcpy(filename2, argv[optind + 1]);
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
		quit_converter(fd);
		quit_converter(fd2);
	}
    
}

void print_help(void) {
	int j = 0;
	for(j = 0; j < 4; j++){
		printf("%s", USAGE[j]); 
	}
	quit_converter(fd);
	quit_converter(fd2);
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

void verb1(char* filename_sh){
	int size;
	float size_final;
	char file_path[PATH_MAX + 1];
	char *ptr;
	struct stat file_size;
	struct utsname os_name;
	char* hostname;
	hostname = (char*) malloc(1024);

	stat(filename_sh, &file_size);
	size = file_size.st_size;
	size_final = (float) size / 1000;
	fprintf(stderr, "Input file size: %.3f kb. \n", size_final);	

	ptr = realpath(filename_sh, file_path);
	fprintf(stderr, "Input file path: %s\n", ptr);

	if(source == BIG)
		fprintf(stderr, "Input file encoding: %s\n", "UTF16-BE");
	else if(source == LITTLE)
		fprintf(stderr, "Input file encoding: %s\n", "UTF16-LE");

	if(conversion == BIG)
		fprintf(stderr, "Output encoding: %s\n", "UTF16-BE");
	else if(conversion == LITTLE)
		fprintf(stderr, "Output encoding: %s\n", "UTF16-LE");

	gethostname(hostname, 1023);
	fprintf(stderr, "Hostmachine: %s\n", hostname);

	uname(&os_name);
	fprintf(stderr, "Operating System: %s\n", os_name.sysname);

	quit_converter(fd);
	quit_converter(fd2);
}

void verb2(char* filename_sh){
	int size;
	float size_final;
	char file_path[PATH_MAX + 1];
	char *ptr;
	struct stat file_size;
	struct utsname os_name;
	char* hostname;
	hostname = (char*) malloc(1024);

	stat(filename_sh, &file_size);
	size = file_size.st_size;
	size_final = (float) size / 1000;
	fprintf(stderr, "Input file size: %.3f kb. \n", size_final);	

	ptr = realpath(filename_sh, file_path);
	fprintf(stderr, "Input file path: %s\n", ptr);

	if(source == BIG)
		fprintf(stderr, "Input file encoding: %s\n", "UTF16-BE");
	else if(source == LITTLE)
		fprintf(stderr, "Input file encoding: %s\n", "UTF16-LE");

	if(conversion == BIG)
		fprintf(stderr, "Output encoding: %s\n", "UTF16-BE");
	else if(conversion == LITTLE)
		fprintf(stderr, "Output encoding: %s\n", "UTF16-LE");

	gethostname(hostname, 1023);
	fprintf(stderr, "Hostmachine: %s\n", hostname);

	uname(&os_name);
	fprintf(stderr, "Operating System: %s\n", os_name.sysname);

	fprintf(stderr, "Reading: %s\n", "STUFF");
	fprintf(stderr, "Converting: %s\n", "STUFF");
	fprintf(stderr, "Writing: %s\n", "STUFF");

	if(ascii_total != 0)
		fprintf(stderr, "ASCII: %.0f%%\n", ((ascii_count / ascii_total) * 100));
	else
		fprintf(stderr, "ASCII: 0\n");

	if(surr_total != 0)
		fprintf(stderr, "Surrogates: %.0f%%\n", ((surr_count / surr_total) * 100));

	fprintf(stderr, "Glyphs: %d\n", glyph_total);

	quit_converter(fd);
	quit_converter(fd2);
	
}