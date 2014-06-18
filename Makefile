json_examples:
	./main.rb -w example/3/weights.init < example/3/in.sgm > example/3/3.json 2>/dev/null
	./main.rb -w example/3/weights.init -l < example/3/in.sgm > example/3/3-with-glue.json 2>/dev/null
	./main.rb -w example/glue/weights -l < example/glue/in.sgm > example/glue/glue.json 2>/dev/null
	./main.rb -w example/toy/weights < example/toy/in.sgm > example/toy/toy.json 2>/dev/null
	./main.rb -w example/toy/weights < example/toy/in-test.sgm > example/toy/toy-test.json 2>/dev/null

