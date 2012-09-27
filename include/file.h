#ifndef MYFILE_H__
#define MYFILE_H__

#define READ_ERROR	0x01
#define WRITE_ERROR	0x02

int copy_file(const char *input, const char *output);
void validate_output(const char *path, char **allowed);


#endif
