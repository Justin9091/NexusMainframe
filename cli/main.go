package main

import (
	"nexus-cli/commands"
	"os"

	"github.com/spf13/cobra"
)

func main() {
	var baseURL string

	root := &cobra.Command{
		Use:   "nexus",
		Short: "Nexus CLI — manage your NexusMainFrame instance",
	}

	root.PersistentFlags().StringVar(&baseURL, "host", "http://localhost:8080", "NexusMainFrame host URL")

	root.AddCommand(commands.NewModulesCmd(&baseURL))

	if err := root.Execute(); err != nil {
		os.Exit(1)
	}
}
