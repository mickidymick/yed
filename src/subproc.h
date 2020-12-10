#ifndef __SUBPROC_H__
#define __SUBPROC_H__


char * yed_run_subproc(char *cmd, int *output_len, int *status);

/*
** The return values of these functions are as follows:
** 0 indicates success.
** In the case that popen or pclose fail, the value returned will
**   be the value of errno at the time of failure.
** In the case that yed_fill_buff_from_file_stream() fails,
**   the value returned will be the negated status value
**   of yed_fill_buff_from_file_stream().
**
** So, when checking the error, do something like this:
**
**     int status;
**     status = yed_read_subproc_into_buffer(cmd, buff, &exit_status);
**     if (status < 0) {
**         if (-status == BUFF_FILL_STATUS_ERR_PER) { ... }
**         // check other BUFF_FILL_STATUS_ERR_* values
**     } else if (status > 0) {
**         printf("popen failed -- errno was %d\n", status);
**     }
*/
int yed_read_subproc_into_buffer(char *cmd, yed_buffer *buff, int *exit_status);
/*
** If a non-null pointer is passed as 'output', then '*output' should be freed
** by the caller when appropriate.
*/
int yed_write_buffer_to_subproc(yed_buffer *buff, char *cmd, int *exit_status, char **output);
#endif
