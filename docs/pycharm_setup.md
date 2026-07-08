# PyCharm Setup

1. Open the repository root in PyCharm.
2. Configure a Python 3.12 interpreter.
3. Run:

```bat
scripts\windows\setup_python_env.bat
```

4. Create a Python run configuration:

```text
Module name: uvicorn
Parameters: python_test_station.app.main:app --host 127.0.0.1 --port 8000 --reload
Working directory: repository root
```

5. Start the C++ Gateway separately:

```bat
scripts\windows\run_cpp_gateway.bat
```

6. Open:

```text
http://127.0.0.1:8000
```

