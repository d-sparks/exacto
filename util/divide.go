package main

import (
	"bufio"
	"flag"
	"fmt"
	"os/exec"
	"strconv"
	"strings"
)

type process struct {
	Label             string
	Stdin             chan string
	Stdout            chan string
	UnderlyingProcess *exec.Cmd
}

func panicIfErr(err error) {
	if err != nil {
		panic(err)
	}
}

func NewProcess(command, label string) process {
	underlyingProcess := exec.Command(command)
	stdinPipe, err := underlyingProcess.StdinPipe()
	panicIfErr(err)
	stdoutPipe, err := underlyingProcess.StdoutPipe()
	panicIfErr(err)

	stdinWriter := bufio.NewWriter(stdinPipe)
	stdinChan := make(chan string)
	go func() {
		for s := range stdinChan {
			fmt.Printf("send %s to %s\n", s, label)
			_, err := stdinWriter.WriteString(s + "\n")
			panicIfErr(err)
			stdinWriter.Flush()
		}
	}()

	stdoutReader := bufio.NewReader(stdoutPipe)
	stdoutChan := make(chan string)
	go func() {
		for {
			line, err := stdoutReader.ReadString('\n')
			fmt.Printf("%s: %s", label, line)
			panicIfErr(err)
			stdoutChan <- line
		}
	}()

	go underlyingProcess.Run()

	return process{
		Label:             label,
		Stdin:             stdinChan,
		Stdout:            stdoutChan,
		UnderlyingProcess: underlyingProcess,
	}
}

type divide struct {
	Total  string
	Moves  int
	Output map[string]string
}

func readDivide(c chan string) divide {
	output := map[string]string{}
	var move, count string
	for {
		line := <-c
		split := strings.Split(line, " ")
		if len(split) >= 2 {
			move, count = split[0], split[1]
			if move == "moves" || count == "moves\n" {
				break
			}
			if move == "Force" {
				continue
			}
			if count == "total\n" {
				count, move = move, "total"
			}
			move = strings.Replace(move, ":", "", -1)
			output[move] = strings.Trim(count, " \n:")
		}
	}
	total := output["total"]
	delete(output, "total")
	return divide{Total: total, Moves: len(output), Output: output}
}

func getDifferingMoves(depth int, p1, p2 process) (plus, differ, minus []string) {
	depthString := strconv.Itoa(depth)
	p1.Stdin <- "divide " + depthString
	p2.Stdin <- "divide " + depthString

	results1 := readDivide(p1.Stdout)
	results2 := readDivide(p2.Stdout)

	for k, v := range results1.Output {
		v2, ok := results2.Output[k]
		if !ok {
			plus = append(plus, k)
		} else if v != v2 {
			differ = append(differ, k)
		}
	}

	for k, v := range results2.Output {
		v1, ok := results1.Output[k]
		if !ok {
			minus = append(minus, k)
		} else if v != v1 {
			differ = append(differ, k)
		}
	}

	return
}

func getDifferingMoveSequence(depth int, p1, p2 process) string {
	plus, differ, minus := getDifferingMoves(depth, p1, p2)
	output := []string{""}
	if depth == 0 || len(plus) > 0 || len(minus) > 0 {
		if len(plus) > 0 {
			output = append(output, "Engine 1 had: "+strings.Join(plus, ", "))
		}
		if len(minus) > 0 {
			output = append(output, "Engine 2 had: "+strings.Join(minus, ", "))
		}
		return strings.Join(output, "\n")
	}

	if len(differ) == 0 {
		return ""
	}

	move := differ[0]

	p1.Stdin <- "usermove " + move
	p2.Stdin <- "usermove " + move
	return "usermove " + move + " " + getDifferingMoveSequence(depth-1, p1, p2)
}

func getEngineProcess(path, label string, headerLines int, commands ...string) process {
	process := NewProcess(path, label)
	process.Stdin <- "force"
	for i := 0; i < headerLines; i++ {
		<-process.Stdout

	}
	for _, command := range commands {
		process.Stdin <- command
	}
	return process
}

func main() {
	engine1 := flag.String("engine1", "", "path to engine1 executable")
	engine2 := flag.String("engine2", "", "path to engine2 executable")
	label1 := flag.String("label1", "engine1", "label for engine1 loglines")
	label2 := flag.String("label2", "engine2", "label for engine2 loglines")
	headers1 := flag.Int("headers1", 7, "number of header loglines to ignore (engine 1)")
	headers2 := flag.Int("headers2", 7, "number of header loglines to ignore (engine 2)")
	commands1 := flag.String("commands1", "force", "comma-separated init commands (engine 1)")
	commands2 := flag.String("commands2", "force", "comma-separated init commands (engine 2)")
	// fenFile := flag.String("fen", "", "path to FEN file")
	// depth := flag.Int("depth", 4, "depth to search each position")
	flag.Parse()

	sliceCommands1 := strings.Split(*commands1, ",")
	sliceCommands2 := strings.Split(*commands2, ",")

	process1 := getEngineProcess(*engine1, *label1, *headers1, sliceCommands1...)
	process2 := getEngineProcess(*engine2, *label2, *headers2, sliceCommands2...)

	fen := "setboard 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - - -"

	// set boards if necessary
	process1.Stdin <- fen
	process2.Stdin <- fen

	fmt.Println("\n" + fen + " " + getDifferingMoveSequence(5, process1, process2))

	// exit
	go func() { process1.Stdin <- "quit" }()
	go func() { process2.Stdin <- "quit" }()
}
