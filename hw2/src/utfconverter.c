#include "utfconverter.h"

int fd, fd2 = 0;
int v_counter = 0;
int num_bytes = 0;
float ascii_count, ascii_total = 0;
float surr_count, surr_total = 0;
int glyph_total = -1; /*We are counting the BOM*/
static clock_t read_start, read_end, write_start, write_end, convert_start, convert_end = 0;
static struct tms read_start2, read_end2, convert_start2, convert_end2, write_start2, write_end2;

int main(int argc, char** argv){
	unsigned char buf[MAX_BYTES];
	int rv = 0;
	Glyph* glyph = NULL;

	/* After calling parse_args(), filename and conversion should be set. */
	parse_args(argc, argv);
	if ((fd = open(filename, O_RDONLY)) == 0)
		return EXIT_FAILURE;
	if(filename2 != NULL)
		fd2 = open(filename2, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IROTH | S_IRGRP
			| S_IWUSR | S_IWGRP | S_IWOTH);

	glyph = malloc(sizeof(Glyph));
	
	/* Handle BOM bytes for UTF16 specially. 
         * Read our values into the first and second elements. */
	read_start = clock();
	times(&read_start2);
	times(&convert_start2);
	times(&write_start2);
	if((rv = read(fd, &buf[0], 1)) == 1 && (rv = read(fd, &buf[1], 1)) == 1){

		rv = read(fd, &buf[2], 1);

		if(buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf){
			source = UTF8;
		}
		else if(buf[0] == 0xfe && buf[1] == 0xff){
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
			return EXIT_FAILURE;
		}
		memset(glyph, 0, sizeof(Glyph));

		if(source != UTF8)
			lseek(fd, -sizeof(unsigned char), SEEK_CUR);
	}

	convert_start = clock();

	if(source != UTF8)
		fill_glyph(glyph, buf, source, &fd);
	else if (source == UTF8){
			buf[0] = 0xff;
			buf[1] = 0xfe;
			fill_glyph(glyph, buf, conversion, &fd);
	}

	if(conversion != LITTLE && source != UTF8)
		swap_endianness(glyph);

	write_start = clock();
	write_glyph(glyph,fd2);


	if(source != UTF8){
	/* Now deal with the rest oconversionf the bytes.*/
		while((rv = read(fd, &buf[0], 1)) == 1 &&
				(rv = read(fd, &buf[1], 1)) == 1){

			glyph = fill_glyph(glyph, buf, source, &fd);

			if(conversion!= LITTLE && source != UTF8)
				glyph = swap_endianness(glyph);

			write_glyph(glyph,fd2);
		}
	}

	else if(source == UTF8){

		while((rv = read(fd, &buf[0], 1)) == 1){

			num_bytes = how_many_bytes(buf);

			if(num_bytes == 1)
				glyph = fill_glyph(glyph, buf, source, &fd);
			else if(num_bytes == 2){
				if((rv = read(fd, &buf[1], 1)) == 1)
					glyph = fill_glyph(glyph, buf, source, &fd);
				else
					print_help();
			}
			else if(num_bytes == 3){
				if((rv = read(fd, &buf[1], 1)) == 1 && (rv = read(fd, &buf[2], 1)) == 1)
					glyph = fill_glyph(glyph, buf, source, &fd);
				else
					print_help();
			}
			else if(num_bytes == 4){
				if((rv = read(fd, &buf[1], 1)) == 1 && (rv = read(fd, &buf[2], 1)) == 1 && 
					(rv = read(fd, &buf[3], 1)) == 1)
					glyph = fill_glyph(glyph, buf, source, &fd);
				else
					print_help();
			}

			glyph = convert(glyph, conversion);
			write_glyph(glyph, fd2);
		}

	}

	read_end = clock();
	convert_end = clock();
	write_end = clock();
	times(&read_end2);
	times(&convert_end2);
	times(&write_end2);

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

int how_many_bytes(unsigned char data[1]){
		if((data[FIRST] >> 7) == 0){
			return 1;
		}
		else{
			if((data[FIRST] >> 5) == 0x06){
				return 2;
			}
			else if((data[FIRST] >> 4) == 0xe){ 
				return 3;
			}
			else if((data[FIRST] >> 3) == 0x1e){
				return 4;
			}
			else{
				print_help(); 
			}
		}
		return 0;
}

Glyph* fill_glyph(Glyph* glyph, unsigned char data[MAX_BYTES], endianness end, int* fd){
	unsigned int bits;

	if(end == LITTLE){
		if(data[0] < 128 && data[1] == 0)
			ascii_count++;
		ascii_total++;
	}
	else if(end == BIG){
		if(data[1] < 128 && data[0] == 0)
			ascii_count++;
		ascii_total++;
	}

	if(end == LITTLE){
		glyph->bytes[0] = data[0];
		glyph->bytes[1] = data[1];
	}
	else if (end == BIG){
		glyph->bytes[0] = data[1]; 
		glyph->bytes[1] = data[0];
	}
	else if(end == UTF8){
		/*fprintf(stderr, "FILL GLYPH: %8x\n", data[FIRST]);
		fprintf(stderr, "HOW MANY BYTES MOVED %d\n", num_bytes);*/
		if(num_bytes == 1){
			glyph->bytes[0] = data[0];
			glyph->bytes[1] = 0;;
			glyph->bytes[2] = 0;
			glyph->bytes[3] = 0;
			glyph->end = end;
			glyph->surrogate = false;
			num_bytes = 1;
			return glyph;
		}
		else{
			if(num_bytes == 2){
				/*fprintf(stderr, "SECOND BYTE %8x\n", data[SECOND]);*/
				glyph->bytes[0] = data[0];
				glyph->bytes[1] = data[1];
				glyph->bytes[2] = 0;
				glyph->bytes[3] = 0;
				glyph->end = end;
				glyph->surrogate = false;
				num_bytes = 2;
				return glyph;
			}
			else if(num_bytes == 3){ 
				/*fprintf(stderr, "SECOND BYTE %8x\n", data[SECOND]);
				fprintf(stderr, "THIRD BYTE: %8x\n", data[THIRD]);*/

				glyph->bytes[0] = data[0];
				glyph->bytes[1] = data[1];
				glyph->bytes[2] = data[2];
				glyph->bytes[3] = 0;
				glyph->end = end;
				glyph->surrogate = false;
				num_bytes = 3;
				return glyph;
			}
			else if(num_bytes == 4){

				glyph->bytes[0] = data[0];
				glyph->bytes[1] = data[1];
				glyph->bytes[2] = data[2];
				glyph->bytes[3] = data[3];
				glyph->end = end;
				glyph->surrogate = true;
				num_bytes = 4;
				return glyph;
			}
			else{
				print_help(); 
			}
		}
	}

	bits = 0; 
	bits |= ((data[FIRST] << 8 ) + data[SECOND]);
	/* Check high surrogate pair using its special value range.*/

	if(bits > 0xD800 && bits < 0xDBFF){ 
		surr_count++;
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
		glyph->bytes[FOURTH] = '\0';
		glyph->bytes[THIRD] = '\0';
	} else {
		if(end == BIG){
			glyph->bytes[THIRD] = data[SECOND]; 
			glyph->bytes[FOURTH] = data[FIRST];
			glyph_total++;
			ascii_total++;
		}
		else{
			glyph->bytes[THIRD] = data[FIRST]; 
			glyph->bytes[FOURTH] = data[SECOND];
		}
	}

	glyph->end = end;
	glyph_total++;
	surr_total++;
	return glyph;
}

void write_glyph(Glyph* glyph, int fd_2){
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
	else if(source == UTF8)
		fprintf(stderr, "Input file encoding: %s\n", "UTF8");

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
	float read_real, convert_real, write_real;
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
	else if (source == UTF8)
		fprintf(stderr, "Input file encoding: %s\n", "UTF8");

	if(conversion == BIG)
		fprintf(stderr, "Output encoding: %s\n", "UTF16-BE");
	else if(conversion == LITTLE)
		fprintf(stderr, "Output encoding: %s\n", "UTF16-LE");

	gethostname(hostname, 1023);
	fprintf(stderr, "Hostmachine: %s\n", hostname);

	uname(&os_name);
	fprintf(stderr, "Operating System: %s\n", os_name.sysname);

	read_real = (float) (read_end - read_start) / CLOCKS_PER_SEC;
	convert_real = (float) (convert_end - convert_start) / CLOCKS_PER_SEC;
	write_real = (float) (write_end - write_start) / CLOCKS_PER_SEC;

	fprintf(stderr, "Reading: real=%.1f, user=%.1f, sys=%.1f\n", read_real,
		(double) (read_end2.tms_utime - read_start2.tms_utime),
		(double) (read_end2.tms_stime - read_start2.tms_stime));
	fprintf(stderr, "Converting: real=%.1f, user=%.1f, sys=%.1f\n", convert_real,
		(double) (convert_end2.tms_utime - convert_start2.tms_utime),
		(double) (convert_end2.tms_stime - convert_start2.tms_stime));
	fprintf(stderr, "Writing: real=%.1f, user=%.1f, sys=%.1f\n", write_real,
		(double) (write_end2.tms_utime - write_start2.tms_utime),
		(double) (write_end2.tms_stime - write_start2.tms_stime)
		);

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

Glyph* convert(Glyph* glyph, endianness end){
	surr_total++;
	ascii_total++;
	glyph_total++;
	if(num_bytes == 1){
		if(end == LITTLE){
			glyph->bytes[0] = glyph->bytes[0];
			glyph->bytes[1] = 0;

			if(glyph->bytes[0] < 128 && glyph->bytes[1] == 0)
				ascii_count++;;

		}else if(end == BIG){
			glyph->bytes[1] = glyph->bytes[0];
			glyph->bytes[0] = 0;

			if(glyph->bytes[1] < 128 && glyph->bytes[0] == 0)
				ascii_count++;
		}
		glyph->bytes[2] = 0;
		glyph->bytes[3] = 0;
	}
	else{
		if(num_bytes == 2){
			unsigned int temp;
			unsigned int temp2;
			temp = 0;

			glyph->bytes[0] = glyph->bytes[0] & 0x1f;
			glyph->bytes[1] = glyph->bytes[1] & 0x3f;

			temp += glyph->bytes[0]; 
			temp = temp << 6;
			temp2 = glyph->bytes[1];
			temp2 = temp2;
			temp += temp2;

			if(end == LITTLE){
				glyph->bytes[0] = temp;
				glyph->bytes[1] = 0;
			}else if(end == BIG){
				glyph->bytes[1] = temp;
				glyph->bytes[0] = 0;
			}

			glyph->bytes[2] = 0;
			glyph->bytes[3] = 0;

		}

		else if(num_bytes == 3){
			unsigned int temp;
			temp = 0;
			a1 = 0; 
			a2 = 0;

			glyph->bytes[0] = glyph->bytes[0] & 0x0f;
			temp += glyph->bytes[0];
			temp = temp << 6;

			glyph->bytes[1] = glyph->bytes[1] & 0x3f;
			temp += glyph->bytes[1];
			temp = temp << 6;

			glyph->bytes[2] = glyph->bytes[2] & 0x3f;
			temp += glyph->bytes[2];

			a1 = (temp & 0xFF00) >> 8;
			a2 = temp & 0x00FF;

			if(end == BIG){
				glyph->bytes[0] = a1;
				glyph->bytes[1] = a2;
			}
			else if(end == LITTLE){
				glyph->bytes[0] = a2;
				glyph->bytes[1] = a1;
			}

			glyph->bytes[2] = 0;
			glyph->bytes[3] = 0;
		}


		else if(num_bytes == 4){
			long mutli, imm1, imm2;
			unsigned int t1, t2;
			ascii_total++;
			surr_count++;
			glyph_total++;
			mutli = 0;
			a3 = 0;
			a4 = 0;
			t1 = 0;
			t2 = 0;

			glyph->bytes[0] = glyph->bytes[0] & 0x07;
			mutli += glyph->bytes[0];
			mutli = mutli << 6;

			glyph->bytes[1] = glyph->bytes[1] & 0x3f;
			mutli += glyph->bytes[1]; 
			mutli = mutli << 6;

			glyph->bytes[2] = glyph->bytes[2] & 0x3f;
			mutli += glyph->bytes[2];
			mutli = mutli << 6;

			glyph->bytes[3] = glyph->bytes[3] & 0x3f;
			mutli += glyph->bytes[3];

			mutli = mutli - 0x10000; /*20 bytes*/

			imm1 = mutli >> 10; /*vh*/
			imm2 = mutli & 0x3FF; /*vl*/

			t1 = imm1 + 0xD800;
			t2 = imm2 + 0xDC00;

			a1 = (t1 & 0xFF00) >> 8;
			a2 = t1 & 0x00FF;

			a4 = (t2 & 0xFF00) >> 8;
			a3 = t2 & 0x00FF;

			if(end == BIG){
				glyph->bytes[0] = a1;
				glyph->bytes[1] = a2;
				glyph->bytes[2] = a4;
				glyph->bytes[3] = a3;
			}
			else if(end == LITTLE){
				glyph->bytes[0] = a2;
				glyph->bytes[1] = a1;
				glyph->bytes[2] = a3;
				glyph->bytes[3] = a4;
			}

		}
	}


	return glyph;
}