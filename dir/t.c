#include "all.h"

int main()
{
		  char buf[] = "hello world\n";
		  struct aiocb aio;
		  aio.aio_nbytes = strlen(buf);
		  aio.aio_buf = buf;
		  aio.aio_fildes = STDOUT_FILENO;
		  aio_write(&aio);
		  sleep(10);
		  return 0;
}
