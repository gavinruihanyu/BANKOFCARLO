#include <cstdlib>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

mongocxx::instance instance{};// don't put inside main 

class user_account
{public:
	std::string user_email;
	std::string user_password;
	std::string account_balance = "0";
};


void welcome_screen()
{
	std::cout << " ******       **     ****     ** **   **         *******   ********         ******      **     *******   **         *******  \n/*////**     ****   /**/**   /**/**  **         **/////** /**/////         **////**    ****   /**////** /**        **/////** \n/*   /**    **//**  /**//**  /**/** **         **     //**/**             **    //    **//**  /**   /** /**       **     //**\n/******    **  //** /** //** /**/****         /**      /**/*******       /**         **  //** /*******  /**      /**      /**\n/*//// ** **********/**  //**/**/**/**        /**      /**/**////        /**        **********/**///**  /**      /**      /**\n/*    /**/**//////**/**   //****/**//**       //**     ** /**            //**    **/**//////**/**  //** /**      //**     ** \n/******* /**     /**/**    //***/** //**       //*******  /**             //****** /**     /**/**   //**/******** //*******  \n///////  //      // //      /// //   //         ///////   //               //////  //      // //     // ////////   ///////   \n";
	std::cout << "\n\n\n";
	std::cout << "------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Welcome to the bank of carlo!\n";
	std::cout << "------------------------------------------------------------------------------------------------------------------------------\n\n\n\n";
	std::cout << "Menu: 1:LOGIN || 2:REGISTER(2)\n";
	std::cout << "\n> ";
}

//will return index of response of array
int menu_val(const std::string* options)
{
	//recurse while option is not valid
	while (true)
	{
		std::string response;
		std::getline(std::cin, response);
		std::cout << "\n";

		for (unsigned int i = 0; i < response.length(); ++i)
		{
			response[i] = std::tolower(response[i]);
		}

		for (unsigned int i = 0; i < sizeof(options); i++)
		{
			if (response == options[i]) return i;
		}

		std::cout << "That is not a valid option, please try again:\n> ";
	}
}

void menu()
{
	int n = 100;
	char* str = new char[n];
	system("CLS");
	std::cout << "------------------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Your Account\n";
	std::cout << "------------------------------------------------------------------------------------------------------------------------------\n\n\n\n";

	std::cin >> str;
	delete(str);
}

int login(mongocxx::collection *collection)
{
	std::string response;
	//document found
	bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result;

	while (true)
	{
		std::cout << "Account Email: ";
		std::getline(std::cin, response);
		std::cout << "\n";
		//add some security shit here or else your databse gonna be hackers playground ^
		
		//find document by field
		maybe_result = collection->find_one(document{} << "account_email" << response << finalize);
		if (maybe_result)	break;

		std::cout << "\nEmail not registered. Try again(1), Create account(2)\n> ";
		std::string options[] =
		{
			"1", "2", "try again", "create account"
		};
		int choice = menu_val(options);

		if (choice == 1 || choice == 3) return 2;
		//set options and invoke menu_val function
	}
	std::cout << "Account Passcode: ";
	unsigned int i = 0;
	while (true)
	{
		if (i == 2) break;
		std::getline(std::cin, response);
		std::cout << "\n";
		bsoncxx::document::view view = maybe_result->view();
		std::string password = bsoncxx::to_json(view);
		std::string match = "account_password\" : \"";
		match.append(response);

		if (password.find(match) != std::string::npos)	return 1;
	
		std::cout << "Invalid password, try again: ";
		i++;
	}
	return 0;

}

void register_account(mongocxx::collection *collection)
{
	user_account new_user;
	std::string input;
	std::cout << "Welcome to Bank Of Carlo, we are pleased to have you!\n";
	while (true)
	{
		std::string confirm;
		std::cout << "Please enter a valid email that you will use to log into your account with: ";
		std::getline(std::cin, input);
		std::cout << "\n";
		std::cout << "Please confirm email: ";
		std::getline(std::cin, confirm);
		if (confirm == input)
		{
			new_user.user_email = input;
			std::cout << "\n";
			break;
		}
		std::cout << "Emails did not match, please try again:\n";
	}
	while (true)
	{
		std::string confirm;
		std::cout << "Please enter a password: ";
		std::getline(std::cin, input);
		std::cout << "\n";
		std::cout << "Please confirm password: ";
		std::getline(std::cin, confirm);
		if (confirm == input)
		{
			new_user.user_password = input; 
			std::cout << "\n";
			break;
		}
		std::cout << "Passwords did not match, please try again:\n";
	}

	//build document
	bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value data_wrote = builder
		<< "account_email" << new_user.user_email
		<< "account_balance" << new_user.account_balance
		<< "account_password" << new_user.user_password
		<< bsoncxx::builder::stream::finalize;
	//moving the data_wrote to data view so we can work with it post creation
	bsoncxx::document::view data = data_wrote.view();
	//insert data to collection and store the result in result 
	bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection->insert_one(data);

}

int main() //dont forget to minimize function stacking 
{		
	while (true)
	{	

		mongocxx::uri uri("mongodb://localhost:27017");
		mongocxx::client client(uri);

		//connect to db then collection
		mongocxx::database db = client["BankOfCarlo"];
		mongocxx::collection accounts = db["accounts"];

		//for welcome screen
		welcome_screen();
		int response;
		std::string option[] = {"1","2", "login", "register"};
		response = menu_val(option);

		system("CLS");

		if (response == 0 || response == 2)//login
		{
			//if user aborts login
			int logged_in = login(&accounts);
			if (!logged_in)	return EXIT_SUCCESS;
			else if (logged_in == 2) register_account(&accounts);
			else if (logged_in == 1) menu();
		}
		if (response == 1 || response == 3)//register
		{
			register_account(&accounts);
		}
	}
	return EXIT_SUCCESS;
}

/*
void connect_to_mongo()
{

	mongocxx::uri uri("mongodb://localhost:27017");
	mongocxx::client client(uri);

	//connect to db then collection
	mongocxx::database db = client["BankOfCarlo"];
	mongocxx::collection accounts = db["accounts"];

	//build document
	bsoncxx::builder::stream::document builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value data_wrote = builder
		<< "name" << "MongoDB"
		<< "type" << "database"
		<< "count" << 1
		<< "versions"
		<< bsoncxx::builder::stream::open_array
		<< "v3.2" << "v3.0" << "v2.6"
		<< bsoncxx::builder::stream::close_array
		<< "info"
		<< bsoncxx::builder::stream::open_document
		<< "x" << 203
		<< "y" << 102
		<< bsoncxx::builder::stream::close_document
		<< bsoncxx::builder::stream::finalize;
	//moving the data_wrote to data view so we can work with it post creation
	bsoncxx::document::view data = data_wrote.view();
	//insert data to collection and store the result in result 
	bsoncxx::stdx::optional<mongocxx::result::insert_one> result = accounts.insert_one(data);
}
*/