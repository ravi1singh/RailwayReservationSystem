/* Software systems mini project : Railway ticket booking system
 */
 

/* Module of admin */

int loginVerify(struct login user);
int addNewCredentials(struct credentials cred);
int addNewAccount(struct account acc);
int deleteAccount(int uid);
int searchAccount(struct account *acc,int uid);
int addNewTrain(struct train newTrain);
int deleteTrain(int trainNo);
int searchTrain(struct train *tr,int trainNo);
int modifyTrain(struct train *tr,int trainNo);
int modifyAccount(struct account *acc,int uid);
int getAllUsers(int uid,int userLockFd,struct dataBase *db);
int getAllTrains(struct dataBase *db);
