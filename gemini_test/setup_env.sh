#!/bin/bash

# Define the base directory
BASE_DIR="/home/claghrab/Desktop/mini-web"

echo "Scaffolding WebServ test environment in $BASE_DIR..."

# ==============================================================================
# 1. SERVER 1: Main Application
# ==============================================================================
mkdir -p "$BASE_DIR/www"
mkdir -p "$BASE_DIR/uploads"
mkdir -p "$BASE_DIR/cgi-bin"

# Index and Error Pages
echo "<h1>Welcome to Server 1 (Main Site)</h1>" > "$BASE_DIR/www/index.html"
echo "<h1>404 - Not Found</h1><p>The router couldn't find this file.</p>" > "$BASE_DIR/www/404.html"
echo "<h1>50x - Server Error</h1><p>Something went wrong.</p>" > "$BASE_DIR/www/50x.html"

# Dummy CGI Script (PHP)
cat << 'EOF' > "$BASE_DIR/cgi-bin/script.php"
#!/usr/bin/php
<?php
echo "Content-Type: text/plain\r\n\r\n";
echo "CGI Script Executed Successfully!\n";
?>
EOF
chmod +x "$BASE_DIR/cgi-bin/script.php"

# ==============================================================================
# 2. SERVER 2: Static Media Server
# ==============================================================================
mkdir -p "$BASE_DIR/shared"
mkdir -p "$BASE_DIR/downloads"

# Alias Test: The location is /assets/, but alias strips it and maps to /shared/
echo "This is a fake image file for the alias test." > "$BASE_DIR/shared/cat.png"

# Root Test: The location is /downloads/, root appends the URI, so it goes in /downloads/
echo "This is a fake zip file for the root test." > "$BASE_DIR/downloads/file.zip"

# ==============================================================================
# 3. SERVER 3: API Server (Virtual Host & Longest Prefix Tests)
# ==============================================================================
# In standard routing, 'root' appends the full URI to the root path.
# This creates the weird nested folders below, which is the perfect test for your C++ string math!

# Location: /api/ -> Root: /api_root -> Full Path: /api_root/api/
mkdir -p "$BASE_DIR/api_root/api"
echo '{"status": "API is online", "server": "Server 3"}' > "$BASE_DIR/api_root/api/status.json"

# Location: /api/public/ -> Root: /api_root/public -> Full Path: /api_root/public/api/public/
mkdir -p "$BASE_DIR/api_root/public/api/public"
echo "This is a public file. Autoindex should be OFF here." > "$BASE_DIR/api_root/public/api/public/readme.txt"

# Location: /api/admin/ -> Root: /api_root/admin -> Full Path: /api_root/admin/api/admin/
mkdir -p "$BASE_DIR/api_root/admin/api/admin"
echo "<h1>Admin Dashboard</h1><p>No deletes allowed here!</p>" > "$BASE_DIR/api_root/admin/api/admin/dashboard.html"

echo "========================================================"
echo "Environment successfully created!"
echo "You can now run: ./webserv [your_config_file.conf]"
echo "========================================================"
