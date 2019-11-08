/**
 * @file      main.cpp
 * @author    Atakan S.
 * @date      01/01/2019
 * @version   1.0
 * @brief     Example project for cpprestsdk using reqres server tutorial.
 *  
 *  atakansarioglu.com/easy-quick-start-cplusplus-rest-client-example-cpprest-tutorial
 *
 * @copyright Copyright (c) 2018 Atakan SARIOGLU ~ www.atakansarioglu.com
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <cpprest/uri.h>
#include <cpprest/json.h>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

int main() {
	// Create a file stream to write the received file into it.
	auto fileStream = std::make_shared<ostream>();
	
	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("users.json"))
	
	// Make a GET request.
	.then([=](ostream outFile) {
		*fileStream = outFile;

		// Create http_client to send the request.
		http_client client(U("https://reqres.in"));

		// Build request URI and start the request.
		return client.request(methods::GET, uri_builder(U("api")).append_path(U("users")).to_string());
	})

	// Get the response.
	.then([=](http_response response) {
		// Check the status code.
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " + std::to_string(response.status_code()));
		}

		// Write the response body to file stream.
		response.body().read_to_end(fileStream->streambuf()).wait();

		// Close the file.
		return fileStream->close();
	});

	// Make a GET request.
	auto requestJson = http_client(U("https://reqres.in"))
		.request(methods::GET,
			uri_builder(U("api")).append_path(U("users")).append_query(U("id"), 1).to_string())

	// Get the response.
	.then([](http_response response) {
		// Check the status code.
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " + std::to_string(response.status_code()));
		}

		// Convert the response body to JSON object.
		return response.extract_json();
	})

	// Get the data field.
	.then([](json::value jsonObject) {
		return jsonObject[U("data")];
	})

	// Parse the user details.
	.then([](json::value jsonObject) {
		std::wcout << jsonObject[U("first_name")].as_string()
			<< " " << jsonObject[U("last_name")].as_string()
			<< " (" << jsonObject[U("id")].as_integer() << ")"
			<< std::endl;
	});

	// Create user data as JSON object and make POST request.
	auto postJson = pplx::create_task([]() {
		json::value jsonObject;
		jsonObject[U("first_name")] = json::value::string(U("atakan"));
		jsonObject[U("last_name")] = json::value::string(U("sarioglu"));

		return http_client(U("https://reqres.in"))
			.request(methods::POST,
				uri_builder(U("api")).append_path(U("users")).to_string(),
				jsonObject.serialize(), U("application/json"));
	})

	// Get the response.
	.then([](http_response response) {
		// Check the status code.
		if (response.status_code() != 201) {
			throw std::runtime_error("Returned " + std::to_string(response.status_code()));
		}

		// Convert the response body to JSON object.
		return response.extract_json();
	})

	// Parse the user details.
	.then([](json::value jsonObject) {
		std::wcout << jsonObject[U("first_name")].as_string()
			<< " " << jsonObject[U("last_name")].as_string()
			<< " (" << jsonObject[U("id")].as_string() << ")"
			<< std::endl;
	});

	// Make PUT request with {"name": "atakan", "location": "istanbul"} data.
	auto putJson = http_client(U("https://reqres.in"))
		.request(methods::PUT,
			uri_builder(U("api")).append_path(U("users")).append_path(U("1")).to_string(),
			U("{\"name\": \"atakan\", \"location\": \"istanbul\"}"),
			U("application/json"))

	// Get the response.
	.then([](http_response response) {
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " + std::to_string(response.status_code()));
		}

		// Convert the response body to JSON object.
		return response.extract_json();
	})

	// Parse the user details.
	.then([](json::value jsonObject) {
		std::wcout << jsonObject[U("name")].as_string()
			<< " " << jsonObject[U("location")].as_string()
			<< std::endl;
	});

	// Make PATCH request with {"name": "sarioglu"} data.
	auto patchJson = http_client(U("https://reqres.in"))
		.request(methods::PATCH,
			uri_builder(U("api")).append_path(U("users")).append_path(U("1")).to_string(),
			U("{\"name\": \"sarioglu\"}"),
			U("application/json"))

	// Get the response.
	.then([](http_response response) {
		if (response.status_code() != 200) {
			throw std::runtime_error("Returned " + std::to_string(response.status_code()));
		}

		// Print the response body.
		std::wcout << response.extract_json().get().serialize() << std::endl;
	});

	// Make DEL request.
	auto deleteJson = http_client(U("https://reqres.in"))
		.request(methods::DEL,
			uri_builder(U("api")).append_path(U("users")).append_path(U("1")).to_string())

	// Get the response.
	.then([](http_response response) {
		std::wcout << "Deleted: " << std::boolalpha << (response.status_code() == 204) << std::endl;
	});

	// Wait for the concurrent tasks to finish.
	try {
		requestJson.wait();
		postJson.wait();
		putJson.wait();
		patchJson.wait();
		deleteJson.wait();
		while (!requestTask.is_done()) { std::cout << "."; }
	} catch (const std::exception &e) {
		printf("Error exception:%s\n", e.what());
	}

	return 0;
}
