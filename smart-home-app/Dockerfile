# --- Build Stage ---
FROM node:22-alpine AS builder
# Upgrade npm
RUN npm install -g npm@latest

WORKDIR /app

# Copy dependencies and install
COPY package*.json ./
RUN npm ci

# Copy the entire app and build it
COPY . .
RUN npm run build

# --- Production Stage ---
FROM nginx:stable-alpine

# Copy built files from the build stage to nginx html directory
COPY --from=builder /app/dist /usr/share/nginx/html

# Add custom nginx configuration
COPY nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
