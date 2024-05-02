# README

To add a new example to the browser demo:
* Duplicate the template folder and rename it
* Replace `template` in the `CMakeLists.txt` and the `template_graph.py` with the name you choose for the folder
* Change the graph to describe the new test. If there is no `OPENCV` tests you can only implement the graph with `OPCNCV=False`
* Add the test to `browser/stream/tests/tests\__init__.py`
* Describe the test in `browser/create_web.py` by adding it to the `TESTS` list
* Add the test to `browser/CMakeLists.txt` : `add_subdirectory(stream/tests/template)`

## Description of the test

Each test is described by an instance of the `Test` class:

```python
def __init__(self,func,cname,title,regenerate=True,hasCV = True):
```

* `func` is the name of the python function used to create the graph. In the template it is name `mk_template`
* `cname` is the name of the folder that is also used as suffix in the name of generated C API to access the tests. In template example it is `template`
* `title` is the name to display in th web pages
* `regenerate` : When developping you may not want to recompute the CMSIS-Stream scheduling for all tests. If regenerate is `False`, the test won't be processed but it will be included in the generated web page. So, the test must already have been processed at least once
* `hasCV` false if the test does not contain a variant with OpenCV

