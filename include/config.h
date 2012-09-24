#ifndef CONFIG_H_
#define CONFIG_H_

struct config {
	char **allowed_paths;
	char *required_user;
	char *required_group;
};

void read_config(const char *cfgfile, struct config *cfg);
void destroy_config(struct config *cfg);

#endif
