#include <cassert>
#include <stdexcept>
#include <iostream>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 
#include <pthread.h>

#include "xmlrpc-c/base.hpp"
#include "xmlrpc-c/registry.hpp"
#include "xmlrpc-c/server_abyss.hpp"

void runInsert(int player, int x, int y);
void runSearch( );
static int callback(void* data, int argc, char **argv, char **azColName) ;
void runUpdate( int x, int y, int player, int game);

using namespace std;

#define SLEEP(seconds) sleep(seconds);
sqlite3 *db;
int playerCount=1;
int game_Number=0;



class joinGame: public xmlrpc_c::method {
public:
    joinGame() {
        // signature and help strings are documentation -- the client
        // can query this information with a system.methodSignature and
        // system.methodHelp RPC.
        this->_signature = "A:ii";
            // method's result and two arguments are integers
        this->_help = "This method joins the game";
    }
    void
    execute(xmlrpc_c::paramList const& paramList,
            xmlrpc_c::value *   const  retvalP) {
        
        int const x(paramList.getInt(0));
        int const y(paramList.getInt(1));

        paramList.verifyEnd(2);
	if((playerCount==1) || (playerCount>4)){
	playerCount=1;
	runInsert(playerCount++,x,y);
	}
	else
	runUpdate(x,y,playerCount++,game_Number-1);

        vector<xmlrpc_c::value> arrayData;
        arrayData.push_back(xmlrpc_c::value_int(game_Number));
        arrayData.push_back(xmlrpc_c::value_double(playerCount));
        arrayData.push_back(xmlrpc_c::value_string("test join"));
        
        // Make an XML-RPC array out of it
        xmlrpc_c::value_array array1(arrayData);
        
        *retvalP = array1;
	runSearch();
        // Sometimes, make it look hard (so client can see what it's like
        // to do an RPC that takes a while).
        
    }
};











////////////////////////////////////////////////////////// SQL Functions

static int callback(void* data, int argc, char **argv, char **azColName) 
{

   fprintf(stderr, "%s: ", (const char*)data);
   for(int i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i]);
   }
   printf("\n");

   return 0;
}

void runUpdate( int x, int y, int player, int game){
	if(!db)
	return;
char *zErrorMsg;
sqlite3_stmt *stmt;
const char *pzTest;
const char *szSQL;
if(player ==1)
szSQL=" UPDATE LOCATIONS set PLAYER1X = ? , PLAYER1Y = ? where GAME_NUMBER=?";
if(player ==2)
szSQL=" UPDATE LOCATIONS set PLAYER2X = ? , PLAYER2Y = ? where GAME_NUMBER=?";
if(player ==3)
szSQL=" UPDATE LOCATIONS set PLAYER3X = ? , PLAYER3Y = ? where GAME_NUMBER=?";
if(player ==4)
szSQL=" UPDATE LOCATIONS set PLAYER4X = ? , PLAYER4Y = ? where GAME_NUMBER=?";
int rc = sqlite3_prepare(db, szSQL, strlen(szSQL),&stmt,&pzTest);
//cout << "The player number is " << player << " the game number is " << game << endl;
if(rc== SQLITE_OK){
sqlite3_bind_int(stmt,1,x);
sqlite3_bind_int(stmt,2,y);
sqlite3_bind_int(stmt,3,game);

}
else
fprintf(stdout, "Update Failure\n");

sqlite3_step(stmt);
sqlite3_finalize(stmt);


}

void runSearch( ){
	if(!db)
	return;
char *zErrMsg=0;
int rc;
const char* sql;
string sql_string;
const char* data= "Callback function called";
sql_string ="SELECT * from LOCATIONS";
sql= sql_string.c_str();
rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
if( rc != SQLITE_OK){
	fprintf(stderr, "SQL error %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
}



}

void runInsert(int player, int x, int y){
	if(!db)
	return;
char *zErrMsg;
sqlite3_stmt *stmt;
const char *pzTest;
const char *szSQL;


if(player ==1)
szSQL="INSERT INTO LOCATIONS (GAME_NUMBER, PLAYER1X, PLAYER1Y, PLAYER2X, PLAYER2Y, PLAYER3X, PLAYER3Y, PLAYER4X, PLAYER4Y) VALUES (?,?, ?, 0,0,0,0,0,0);";
if(player ==2)
szSQL="INSERT INTO LOCATIONS (GAME_NUMBER, PLAYER1X, PLAYER1Y, PLAYER2X, PLAYER2Y, PLAYER3X, PLAYER3Y, PLAYER4X, PLAYER4Y) VALUES (?,0,0, ?, ?,0,0,0,0);";
if(player ==3)
szSQL="INSERT INTO LOCATIONS (GAME_NUMBER, PLAYER1X, PLAYER1Y, PLAYER2X, PLAYER2Y, PLAYER3X, PLAYER3Y, PLAYER4X, PLAYER4Y) VALUES (?, 0, 0, 0, 0, ?, ?, 0, 0);";
if(player ==4)
szSQL="INSERT INTO LOCATIONS (GAME_NUMBER, PLAYER1X, PLAYER1Y, PLAYER2X, PLAYER2Y, PLAYER3X, PLAYER3Y, PLAYER4X, PLAYER4Y) VALUES (?,0,0,0,0,0,0,?,?);";
   
int rc = sqlite3_prepare(db, szSQL, strlen(szSQL),&stmt,&pzTest);
if(rc== SQLITE_OK){
sqlite3_bind_int(stmt,1,game_Number++);
sqlite3_bind_int(stmt,2,x);
sqlite3_bind_int(stmt,3,y);

}
sqlite3_step(stmt);
sqlite3_finalize(stmt);
fprintf(stdout, "Insert Run\n");

}


void deleteEverything(int game){

char *zErrMsg;
sqlite3_stmt *stmt;
const char *pzTest;
const char *szSQL;

//DELETE FROM LOCATIONS
szSQL="DELETE FROM LOCATIONS";
 int rc = sqlite3_prepare(db, szSQL, strlen(szSQL),&stmt,&pzTest);
if(rc== SQLITE_OK){
sqlite3_bind_int(stmt,1,game);



}
sqlite3_step(stmt);
sqlite3_finalize(stmt);
}




















///////////////////////////////////////////////////////


int main(int const,  const char ** const) {


char *zErrMsg=0;
int rc;
const char* sql;
string sql_string;
const char* data= "Callback function called";
 rc= sqlite3_open("locations.db",&db); // open location database

if(rc){
	fprintf(stderr,"Cant open database %s\n",sqlite3_errmsg(db));
	return 0;
}
else{
	fprintf(stderr, "Opened Database successfully\n");
	}
	

//RunUpdate(  70,75,3,3);
//runInsert(2,4,4);
deleteEverything(0);
runSearch();



////////////////////////////////////////
    try {
        xmlrpc_c::registry myRegistry;

     
	xmlrpc_c::methodPtr const sampleJoinGame(new joinGame);
       
	myRegistry.addMethod("server.joinGame", sampleJoinGame);
        
        xmlrpc_c::serverAbyss myAbyssServer(
            xmlrpc_c::serverAbyss::constrOpt()
            .registryP(&myRegistry)
            .portNumber(8080));
        
        myAbyssServer.run();
        // xmlrpc_c::serverAbyss.run() never returns
        assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }
sqlite3_close(db);
    return 0;
}
