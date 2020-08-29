package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"path"
	"text/template"
)

type jsonData struct {
	Namespace       string     `json:"namespace"`
	InlineNamespace string     `json:"inline_namespace"`
	Constants       []constant `json:"constants"`
	Enums           []enum     `json:"enums"`
	Structs         []cStruct  `json:"structs"`
}

type constant struct {
	Definitions []*constantDef `json:definitions`
}

type constantDef struct {
	Name  string `json:"name"`
	Type  string `json:"datatype"`
	Value string `json:"value"`
}

type enum struct {
	ID       string     `json:"identifier"`
	Type     string     `json:"datatype"`
	EnumList []*enumDef `json:"enumerator_list"`
}

type enumDef struct {
	Name    string `json:"name"`
	Comment string `json:"comment"`
}

type cStruct struct {
	ID      string        `json:"identifier"`
	Packed  bool          `json:"packed"`
	Members []*memberDecl `json:"members"`
}

type memberDecl struct {
	Name string `json:"name"`
	Type string `json:"datatype"`
	Size int    `json:"size"`
}

// command line options
var debug bool
var outputFilename string
var templateDir string

func init() {
	const (
		usageFmt = "usage: %s [-d] [-h] [-output <filename>] [-templatedir <path>] <filename>\n"
	)

	flag.BoolVar(&debug, "d", false, "print debug information")
	flag.StringVar(&outputFilename, "output", "/dev/stdout", "output `filename`")
	flag.StringVar(&templateDir, "templatedir", "template", "`path` to templates")

	flag.Usage = func() {
		fmt.Fprintf(flag.CommandLine.Output(), usageFmt, path.Base(os.Args[0]))
		flag.PrintDefaults()
		os.Exit(2)
	}
}

func main() {
	flag.Parse()

	if len(flag.Args()) == 0 {
		fmt.Fprintf(flag.CommandLine.Output(), "error: missing <filename> argument\n\n")
		flag.Usage()
	} else if len(flag.Args()) > 1 {
		fmt.Fprintf(flag.CommandLine.Output(), "error: too many arguments\n\n")
		flag.Usage()
	}

	log.SetFlags(0)
	inputFilename := flag.Args()[0]
	templateName := "cpp_file"

	if debug {
		fmt.Printf("input file:    %s\n", inputFilename)
		fmt.Printf("output file:   %s\n", outputFilename)
		fmt.Printf("templates dir: %s\n", templateDir)
		fmt.Printf("template name: %s\n", templateName)
	}

	tmplSet, err := template.ParseGlob(path.Join(templateDir, "*.tmpl"))
	if err != nil {
		log.Fatalf("error: %v\n", err)
	}

	jsonFile, err := os.Open(inputFilename)
	if err != nil {
		log.Fatalf("error: %v\n", err)
	}
	defer jsonFile.Close()

	var inputdata jsonData
	if err := json.NewDecoder(jsonFile).Decode(&inputdata); err != nil {
		log.Fatalf("error: %v\n", err)
	}

	tmpl := tmplSet.Lookup(templateName + ".tmpl")
	if tmpl == nil {
		log.Fatalf("error: template name '%s' not found\n", templateName)
	}

	cppFile, err := os.Create(outputFilename)
	if err != nil {
		log.Fatalf("error: %v\n", err)
	}
	defer cppFile.Close()

	if err := tmpl.Execute(cppFile, inputdata); err != nil {
		log.Fatalf("error: template execution failed: %v\n", err)
	}
}
