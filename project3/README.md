### 0. Compile the project:
- Go to **project3.rmi\src\main\java**
- Run `sh compile.sh`.

### 1. Start a remote object registry:
- Run  `rmiregistry [PORT_NUMBER] &`,  where PORT_NUMBER is the choosen port (1099 by default).

### 2. Run the server:
- Go to **project3.rmi\src\main\java** (on a new terminal window)
- Run `java Server [-p PORT_NUMBER] [-a ADDRESS]`

### 3. Run the client:
- Go to **project3.rmi\src\main\java** (on yet another terminal window)
- Run `java Client [-p PORT_NUMBER] [-a ADDRESS]`

### Notes:
- Both **-p** and **-a** are optional arguments
- If you set a **PORT_NUMBER** when running `rmiregistry` then **-p** should be passed with it
- **ADDRESS** is **localhost** by default
- You should wait a few seconds after running **rmiregistry** before starting the server, or it may raise a ConnectException
