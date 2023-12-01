from subprocess import Popen, CREATE_NEW_CONSOLE

# Use python bin from the venv. Replace with "python" to use your own installation
python_bin = ".venv/Scripts/python"

# Start process to periodically create the plots from the ouputfile
Popen([python_bin, "createPlot.py"],creationflags=CREATE_NEW_CONSOLE)


# Start process for the webserver
Popen([python_bin, "flask_server.py"],creationflags=CREATE_NEW_CONSOLE)