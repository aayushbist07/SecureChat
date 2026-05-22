#include<iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <cmath>
using namespace std;
class Keys{

    private:

        int p,g;// p n g don't need to be private necessarily, tbh i wanted private key to be const
        int privateKey; 
        string roomKey; // The room key is going to be encrypted by the same algorithm as message is encrypted ig
        int publicKey;
        
    public:

    Keys() {
            privateKey = rand() % 100 + 1; // Random private key between 1 and 100, the probablity of this being 
    }
    string generateroomKey() {
        srand(time(NULL));
        roomKey="";
        string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/!@#$%^&*()_=,.<>?;:[]{}|~!";
        for (int i=0;i<26;i++) {
            roomKey+=chars[rand() % chars.size()];
        }
        return roomKey;
    }
    void setPG(int p,int g){
        this->p=p;
        this->g=g;
    }
    long long modPow(long long base, long long exp, long long mod) {
        long long result = 1;
        base = base % mod;

        while (exp > 0) {
            if (exp % 2 == 1)
                result = (result * base) % mod;

            base = (base * base) % mod;
            exp /= 2;
        }

        return result;
    }
    int generatePublicKey() {
        return modPow(g, privateKey, p);
    }
    string encryptMessage(string message){
        string encryptedMessage="";
        for(int i=0; i<message.length();i++){
            encryptedMessage+=message[i] ^ roomKey[i];
        }
        return encryptedMessage;
    }

    string decryptMessage(string message,string key){
        string decryptedMessage="";
        for(int i=0; i<message.length();i++){
            decryptedMessage+=message[i] ^ key[i];
        }
        return decryptedMessage;
    }

};
