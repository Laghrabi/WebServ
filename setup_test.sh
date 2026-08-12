#!/bin/bash

BASE_DIR="/home/claghrab/Desktop/mini-web/server-tests"

echo "Creating evaluation environment in $BASE_DIR..."

# Create the base directory
mkdir -p "$BASE_DIR"

# ==============================================================================
# ENVIRONMENT 1 & 2: Basic & Standard Configs
# ==============================================================================
echo "Setting up Basic and Standard directories..."
mkdir -p "$BASE_DIR/www"
mkdir -p "$BASE_DIR/uploads"
mkdir -p "$BASE_DIR/cgi-bin"

echo "<h1>Welcome to the Basic Server</h1>" > "$BASE_DIR/www/index.html"
echo "<h1>404 - File Not Found</h1>" > "$BASE_DIR/www/404.html"
echo "This is a dummy file to test autoindex." > "$BASE_DIR/uploads/test_file.txt"

# Standard CGI Script
cat << 'EOF' > "$BASE_DIR/cgi-bin/script.php"
#!/usr/bin/php
<?php
echo "Content-Type: text/plain\r\n\r\n";
echo "Standard CGI Executed Successfully!\n";
?>
EOF
chmod +x "$BASE_DIR/cgi-bin/script.php"


# ==============================================================================
# ENVIRONMENT 3: Multi-Server Basic
# ==============================================================================
echo "Setting up Multi-Server directories..."
mkdir -p "$BASE_DIR/server1"
mkdir -p "$BASE_DIR/server2"
mkdir -p "$BASE_DIR/server3"

echo "<h1>Server 1 (Port 8080 - Default)</h1>" > "$BASE_DIR/server1/index.html"
echo "<h1>Server 2 (Port 8080 - api.local)</h1>" > "$BASE_DIR/server2/index.html"
echo "<h1>Server 3 (Port 8081 - media.local)</h1>" > "$BASE_DIR/server3/index.html"


# ==============================================================================
# ENVIRONMENT 4: The Stress Test
# ==============================================================================
echo "Setting up Stress Test directories..."

# --- Main App (Server 1) ---
mkdir -p "$BASE_DIR/main/errors"
mkdir -p "$BASE_DIR/main/uploads"
mkdir -p "$BASE_DIR/main/cgi-bin"

echo "<h1>Main Web App</h1>" > "$BASE_DIR/main/index.html"
echo "<h1>404 - Custom Error Page</h1>" > "$BASE_DIR/main/errors/404.html"

cat << 'EOF' > "$BASE_DIR/main/cgi-bin/script.php"
#!/usr/bin/php
<?php
echo "Content-Type: text/plain\r\n\r\n";
echo "Stress Test CGI Executed Successfully!\n";
?>
EOF
chmod +x "$BASE_DIR/main/cgi-bin/script.php"


# --- API Server (Server 2 - Virtual Host) ---
# TRAP: Because root is "$BASE_DIR/api", requesting "/api/" appends the URI.
# The physical path becomes "$BASE_DIR/api/api/"
mkdir -p "$BASE_DIR/api/api"
echo '{"status": "API is online", "version": "1.0"}' > "$BASE_DIR/api/api/status.json"

# TRAP: The readonly location appends "/api/readonly/" to the root.
mkdir -p "$BASE_DIR/api/api/readonly"
echo '{"status": "Read Only Mode", "message": "No POSTs allowed"}' > "$BASE_DIR/api/api/readonly/data.json"

# TRAP: Alias test. Requesting "/assets/" maps directly to "$BASE_DIR/shared_assets/"
mkdir -p "$BASE_DIR/shared_assets"
echo "If you see this, your alias math works perfectly!" > "$BASE_DIR/shared_assets/success.txt"


# --- Legacy Server (Server 3 - Redirections) ---
mkdir -p "$BASE_DIR/legacy"
mkdir -p "$BASE_DIR/legacy/docs"

echo "<h1>Legacy Server Home</h1>" > "$BASE_DIR/legacy/default.html"
echo "This is the new documentation folder." > "$BASE_DIR/legacy/docs/readme.txt"

echo "========================================================"
echo "Environment successfully generated at:"
echo "$BASE_DIR"
echo "You are ready for your evaluation!"
echo "========================================================"