#define _GNU_SOURCE

#include <elf.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char **args)
{
	int fd = 0;
    int ret = 0;
    Elf32_Ehdr *elf_header;

    if(argc < 2)
        return 1;

    if((fd = open(args[1], O_RDWR)) == -1)
    	return 1;

    elf_header = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr));

    ret = read(fd, elf_header, sizeof(Elf32_Ehdr));
    if(ret < 1)
    {
    	free(elf_header);
    	close(fd);
    	return 1;
    }

    elf_header->e_shoff = 0xffff;
    elf_header->e_shentsize = 0xffff;
    elf_header->e_shnum = 0xffff;
    elf_header->e_shstrndx = 0xffff;

    lseek(fd, 0, SEEK_SET);

    ret = write(fd, elf_header, sizeof(Elf32_Ehdr));
    if(ret < 1)
    {
    	free(elf_header);
    	close(fd);
    	return 1;
    }

    free(elf_header);
    close(fd);
    return 0;
}
