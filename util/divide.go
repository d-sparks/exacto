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

func printMany(c chan string) {
	for s := range c {
		fmt.Print(s)
	}
}

func readDivide(c chan string) map[string]string {
	out := map[string]string{}
	var move, count string
	for move != "moves" {
		line := <-c
		split := strings.Split(line, " ")
		if len(split) >= 2 {
			move, count = split[0], split[1]
			out[move] = count
		}
	}
	return out
}

// returns "DIFFER" if the moves in this position are differen't, and "SAME" if the totals match.
func getDifferingMove(depth int, p1, p2 process) string {
	depthString := strconv.Itoa(depth)
	p1.Stdin <- "divide " + depthString
	p2.Stdin <- "divide " + depthString

	results1 := readDivide(p1.Stdout)
	results2 := readDivide(p2.Stdout)

	if results1["total"] == results2["total"] {
		return "SAME"
	}
	delete(results1, "total")
	delete(results2, "total")

	if results1["moves"] != results2["moves"] {
		return "DIFFER"
	}

	for k, v := range results1 {
		v2 := results2[k]
		if v != v2 {
			return k
		}
	}

	for k, v := range results2 {
		v2 := results1[k]
		if v != v2 {
			return k
		}
	}

	return "WTF"
}

func getDifferingMoveSequence(depth int, p1, p2 process) string {
	difference := getDifferingMove(depth, p1, p2)
	if difference == "SAME" {
		return "SAME"
	}
	if difference == "DIFFER" {
		return "DIFFER"
	}
	fmt.Printf("Depth %d, difference %s\n", depth, difference)

	p1.Stdin <- "usermove " + difference
	p2.Stdin <- "usermove " + difference
	fmt.Println(difference)
	return difference + " " + getDifferingMoveSequence(depth-1, p1, p2)
}

func getEngineProcess(path, label string, headerLines int, commands ...string) process {
	process := NewProcess(path, label)
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
	headers2 := flag.Int("headers2", 9, "number of header loglines to ignore (engine 2)")
	commands1 := flag.String("commands1", "force", "comma-separated init commands (engine 1)")
	commands2 := flag.String("commands2", "force", "comma-separated init commands (engine 2)")
	// fenFile := flag.String("fen", "", "path to FEN file")
	// depth := flag.Int("depth", 4, "depth to search each position")
	flag.Parse()

	sliceCommands1 := strings.Split(*commands1, ",")
	sliceCommands2 := strings.Split(*commands2, ",")

	process1 := getEngineProcess(*engine1, *label1, *headers1, sliceCommands1...)
	process2 := getEngineProcess(*engine2, *label2, *headers2, sliceCommands2...)

	// set boards if necessary

	fmt.Println(getDifferingMoveSequence(3, process1, process2))

	// exit
	process1.Stdin <- "exit"
	process2.Stdin <- "exit"
}
