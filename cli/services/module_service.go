package services

import (
	"encoding/json"
	"fmt"
	"net/http"
	"nexus-cli/models"
	"strings"
)

type ModuleService struct {
	baseURL string
	client  *http.Client
}

func NewModuleService(baseURL string) *ModuleService {
	return &ModuleService{
		baseURL: baseURL,
		client:  &http.Client{},
	}
}

type apiActionResponse struct {
	Success bool   `json:"success"`
	Output  string `json:"output"`
}

func (s *ModuleService) List(_ models.ListModulesRequest) (*models.ListModulesResponse, error) {
	resp, err := s.client.Get(s.baseURL + "/api/modules")
	if err != nil {
		return nil, fmt.Errorf("cannot reach daemon: %w", err)
	}
	defer resp.Body.Close()

	var modules []models.Module
	if err := json.NewDecoder(resp.Body).Decode(&modules); err != nil {
		return nil, fmt.Errorf("invalid response: %w", err)
	}

	return &models.ListModulesResponse{Modules: modules}, nil
}

func (s *ModuleService) Enable(req models.EnableModuleRequest) (*models.ModuleActionResponse, error) {
	return s.postAction(fmt.Sprintf("/api/modules/%s/enable", req.Name))
}

func (s *ModuleService) Disable(req models.DisableModuleRequest) (*models.ModuleActionResponse, error) {
	return s.postAction(fmt.Sprintf("/api/modules/%s/disable", req.Name))
}

func (s *ModuleService) postAction(path string) (*models.ModuleActionResponse, error) {
	resp, err := s.client.Post(s.baseURL+path, "application/json", strings.NewReader(""))
	if err != nil {
		return nil, fmt.Errorf("cannot reach daemon: %w", err)
	}
	defer resp.Body.Close()

	var raw apiActionResponse
	if err := json.NewDecoder(resp.Body).Decode(&raw); err != nil {
		return nil, fmt.Errorf("invalid response: %w", err)
	}

	if !raw.Success {
		return nil, fmt.Errorf(raw.Output)
	}

	return &models.ModuleActionResponse{Success: true, Message: raw.Output}, nil
}
