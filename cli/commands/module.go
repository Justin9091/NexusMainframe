package commands

import (
	"bytes"
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
	"runtime"
	"strings"

	"github.com/spf13/cobra"
)

type nexusManifest struct {
	Name         string   `json:"name"`
	DisplayName  string   `json:"display_name"`
	Version      string   `json:"version"`
	Description  string   `json:"description"`
	Author       string   `json:"author"`
	License      string   `json:"license"`
	Platforms    []string `json:"platforms"`
	NexusVersion string   `json:"nexus_version"`
	EntryPoint   string   `json:"entry_point"`
}

func NewModuleCmd() *cobra.Command {
	cmd := &cobra.Command{
		Use:   "module",
		Short: "Module development utilities",
	}
	cmd.AddCommand(newModuleInitCmd())
	return cmd
}

func newModuleInitCmd() *cobra.Command {
	var (
		name         string
		displayName  string
		version      string
		description  string
		author       string
		license      string
		platforms    []string
		nexusVersion string
		entryPoint   string
		output       string
		force        bool
	)

	cmd := &cobra.Command{
		Use:   "init",
		Short: "Generate a nexus.json manifest for a module",
		RunE: func(cmd *cobra.Command, args []string) error {
			if name == "" {
				return fmt.Errorf("--name is required")
			}

			if displayName == "" {
				displayName = toDisplayName(name)
			}
			if entryPoint == "" {
				entryPoint = toDLLName(name)
			}
			if len(platforms) == 0 {
				platforms = defaultPlatforms()
			}

			manifest := nexusManifest{
				Name:         name,
				DisplayName:  displayName,
				Version:      version,
				Description:  description,
				Author:       author,
				License:      license,
				Platforms:    platforms,
				NexusVersion: nexusVersion,
				EntryPoint:   entryPoint,
			}

			var buf bytes.Buffer
			enc := json.NewEncoder(&buf)
			enc.SetEscapeHTML(false)
			enc.SetIndent("", "  ")
			if err := enc.Encode(manifest); err != nil {
				return fmt.Errorf("failed to marshal manifest: %w", err)
			}
			data := buf.Bytes()

			if output == "" {
				output = "nexus.json"
			}

			if !force {
				if _, err := os.Stat(output); err == nil {
					return fmt.Errorf("%s already exists; use --force to overwrite", output)
				}
			}

			dir := filepath.Dir(output)
			if dir != "." {
				if err := os.MkdirAll(dir, 0o755); err != nil {
					return fmt.Errorf("failed to create directory: %w", err)
				}
			}

			if err := os.WriteFile(output, data, 0o644); err != nil {
				return fmt.Errorf("failed to write %s: %w", output, err)
			}

			fmt.Printf("Generated %s\n", output)
			return nil
		},
	}

	cmd.Flags().StringVar(&name, "name", "", "Module name, e.g. nexus-chromecast (required)")
	cmd.Flags().StringVar(&displayName, "display-name", "", "Human-readable name (default: derived from --name)")
	cmd.Flags().StringVar(&version, "version", "1.0.0", "Semantic version")
	cmd.Flags().StringVar(&description, "description", "", "Short description of the module")
	cmd.Flags().StringVar(&author, "author", "", "Author name or GitHub handle")
	cmd.Flags().StringVar(&license, "license", "MIT", "SPDX license identifier")
	cmd.Flags().StringSliceVar(&platforms, "platforms", nil, "Target platforms: windows,linux (default: current OS)")
	cmd.Flags().StringVar(&nexusVersion, "nexus-version", ">=1.0.0", "Required NexusMainFrame version constraint")
	cmd.Flags().StringVar(&entryPoint, "entry-point", "", "DLL/SO filename (default: derived from --name)")
	cmd.Flags().StringVar(&output, "output", "nexus.json", "Output file path")
	cmd.Flags().BoolVar(&force, "force", false, "Overwrite existing nexus.json")

	_ = cmd.MarkFlagRequired("name")

	return cmd
}

func toDisplayName(name string) string {
	name = strings.TrimPrefix(name, "nexus-")
	parts := strings.Split(name, "-")
	for i, p := range parts {
		if len(p) > 0 {
			parts[i] = strings.ToUpper(p[:1]) + p[1:]
		}
	}
	return strings.Join(parts, " ") + " Module"
}

func toDLLName(name string) string {
	parts := strings.Split(name, "-")
	result := ""
	for _, p := range parts {
		if len(p) > 0 {
			result += strings.ToUpper(p[:1]) + p[1:]
		}
	}
	ext := ".so"
	if runtime.GOOS == "windows" {
		ext = ".dll"
	}
	return result + ext
}

func defaultPlatforms() []string {
	if runtime.GOOS == "windows" {
		return []string{"windows", "linux"}
	}
	return []string{"linux", "windows"}
}
