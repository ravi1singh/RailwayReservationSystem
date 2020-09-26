/* Software systems mini project : Railway ticket booking system
 */
void doLogin(struct clientRequest *creq,struct serverAck *serAck);
int releaseLock(struct flock *fl,int fd);
int acquireLock(struct flock *fl,int uid);
void flushBuffer();

