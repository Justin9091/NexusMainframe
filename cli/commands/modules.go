package commands

import (
	"fmt"
	"nexus-cli/models"
	"nexus-cli/services"

	"github.com/spf13/cobra"
)

func NewModulesCmd(baseURL *string) *cobra.Command {
	cmd := &cobra.Command{
		Use:   "modules",
		Short: "Manage modules",
	}

	cmd.AddCommand(
		newListCmd(baseURL),
		newEnableCmd(baseURL),
		newDisableCmd(baseURL),
	)

	return cmd
}

func newListCmd(baseURL *string) *cobra.Command {
	return &cobra.Command{
		Use:   "list",
		Short: "List loaded modules",
		RunE: func(cmd *cobra.Command, args []string) error {
			svc := services.NewModuleService(*baseURL)
			resp, err := svc.List(models.ListModulesRequest{})
			if err != nil {
				return err
			}

			if len(resp.Modules) == 0 {
				fmt.Println("No modules loaded.")
				return nil
			}

			for _, m := range resp.Modules {
				fmt.Println(m.Name)
			}
			return nil
		},
	}
}

func newEnableCmd(baseURL *string) *cobra.Command {
	return &cobra.Command{
		Use:   "enable <name>",
		Short: "Enable a module",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			svc := services.NewModuleService(*baseURL)
			resp, err := svc.Enable(models.EnableModuleRequest{Name: args[0]})
			if err != nil {
				return err
			}
			fmt.Println(resp.Message)
			return nil
		},
	}
}

func newDisableCmd(baseURL *string) *cobra.Command {
	return &cobra.Command{
		Use:   "disable <name>",
		Short: "Disable a module",
		Args:  cobra.ExactArgs(1),
		RunE: func(cmd *cobra.Command, args []string) error {
			svc := services.NewModuleService(*baseURL)
			resp, err := svc.Disable(models.DisableModuleRequest{Name: args[0]})
			if err != nil {
				return err
			}
			fmt.Println(resp.Message)
			return nil
		},
	}
}
