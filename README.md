# C++ URL Shortener

A backend URL shortener service built using Crow and SQLite.

## Features
- URL shortening
- HTTP redirect
- Click analytics

## Tech Stack
- C++
- Crow
- SQLite
- CMake

## API Endpoints

Shorten URL
GET /shorten?url=<original_url>

Redirect
GET /<short_code>

Analytics
GET /analytics/<short_code>

## Run Project

mkdir build
cd build
cmake ..
make
./server