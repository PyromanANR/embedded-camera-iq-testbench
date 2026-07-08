.PHONY: install build-zephyr run-zephyr build-cpp run-gateway run-station test samples

install:
	python -m pip install -r python_test_station/requirements.txt

build-zephyr:
	scripts/windows/build_zephyr.bat

run-zephyr:
	scripts/windows/run_zephyr.bat

build-cpp:
	scripts/windows/build_cpp_gateway.bat

run-gateway:
	scripts/windows/run_cpp_gateway.bat

run-station:
	cd python_test_station && python -m uvicorn app.main:app --host 127.0.0.1 --port 8000 --reload

test:
	python -m pytest python_test_station/tests

samples:
	python scripts/create_sample_images.py
