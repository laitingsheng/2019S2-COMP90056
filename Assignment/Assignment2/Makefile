main:
	CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -a -i -tags netgo -ldflags '-s -w -extldflags "-static"' -v main.go
