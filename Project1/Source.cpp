#include <cstdlib>
#include <iostream>
#include <string>
#include <stdlib.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>


using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

mongocxx::instance instance{};// don't put inside main 

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

void menu();

int login() 
{
	std::string reponse;
	while (true)
	{
		std::cout << "Account Email: ";
		std::getline(std::cin, reponse);



	}
	return EXIT_SUCCESS;

}

int register_account()
{

	return EXIT_SUCCESS;
}


int main() 
{
	mongocxx::uri uri("mongodb://localhost:27017");
	mongocxx::client client(uri);

	//for welcome screen
	int response;
	std::string option[] = { "1","2", "login", "register" };

	//response = menu_val(option);

	system("CLS");
	/*if (response == 0 || response == 2)//login
	{
		//if user aborts login
		if (!login()) return EXIT_SUCCESS;
		menu();
	}
	else if (response == 1 || response == 3)//register
	{
		//if user aborts registeration
		if (!register_account())	return EXIT_SUCCESS;
		menu();
	}*/

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

	return EXIT_SUCCESS;
}